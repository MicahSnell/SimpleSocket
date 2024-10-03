/**
 * @file Socket.cpp
 * implementation file for Socket class
 */

#include <iostream>
#include <stdexcept>
#include <cstring>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include "Socket.h"

namespace simple_socket {

Socket::Socket (int portNum, eProtocol protocol)
  : mPort (portNum),
    mProtocol (protocol),
    mSocketFD (CreateSocket ()),
    mIsConnected (false),
    mIsHostSocket (true)
{
  try {
    if (mSocketFD == -1) {
      throw std::runtime_error ("failed to get file descriptor");
    }

    switch (mProtocol) {
      case TCP:
        BindSocket (mPort);
        break;
      case UDP:
        mIsConnected = (BindSocket (mPort) == 0);
        break;
      default:
        throw std::runtime_error ("unknown protocol");
        break;
    }

    #ifdef DEBUG
    std::cout << "Socket: created host socket, is connected: " << std::boolalpha
              << mIsConnected << std::endl;
    #endif
  } catch (std::exception& e) {
    std::cerr << "Error (socket): " << e.what () << std::endl;
    throw;
  }
}

Socket::Socket (std::string hostStr, int portNum, eProtocol protocol)
  : mHostname (hostStr),
    mPort (portNum),
    mProtocol (protocol),
    mSocketFD (CreateSocket ()),
    mIsConnected (mProtocol == UDP),
    mIsHostSocket (false)
{
  try {
    if (mSocketFD == -1) {
      throw std::runtime_error ("failed to get file descriptor");
    }

    // get host info and set in sockaddr_in struct
    hostent* hostEntry = gethostbyname (mHostname.c_str ());
    if (! hostEntry) {
      throw std::runtime_error ("failed to retrieve host info for " + mHostname);
    }
    memset ((char *)& mHostInfo, 0, sizeof (mHostInfo));
    mHostInfo.sin_family = AF_INET;
    mHostInfo.sin_port = htons (mPort);
    mHostInfo.sin_addr = *(in_addr *) hostEntry->h_addr;

    #ifdef DEBUG
    std::cout << "Socket: created client socket, is connected: " << std::boolalpha
              << mIsConnected << std::endl;
    #endif
  } catch (std::exception& e) {
    std::cerr << "Error (socket): " << e.what () << std::endl;
    throw;
  }
}

Socket::Socket (Socket&& other)
{
  mHostname = std::move (other.mHostname);
  mPort = std::move (other.mPort);
  mProtocol = std::move (other.mProtocol);
  mSocketFD = std::move (other.mSocketFD);
  mIsConnected = std::move (other.mIsConnected);
  mIsHostSocket = std::move (other.mIsHostSocket);
  mHostInfo = std::move (other.mHostInfo);

  other.mSocketFD = -1;
}

Socket::~Socket ()
{
  CloseSocket ();
}

bool Socket::Send (const void* buffer, int numBytes)
{
  while (IsNotConnected ()) {
    sleep (1);
    ConnectSocket ();
  }

  try {
    int numBytesSent = -1;
    switch (mProtocol) {
      case TCP:
        numBytesSent = send (mSocketFD, (char *) buffer, numBytes, MSG_NOSIGNAL);
        break;
      case UDP:
        numBytesSent = sendto (mSocketFD, (char *) buffer, numBytes, 0,
                               (sockaddr *)& mHostInfo, sizeof (mHostInfo));
        break;
      default:
        break;
    }

    // if send returns error, of if TCP and sent 0, close to reconnect
    if ((numBytesSent == -1) || ((mProtocol == TCP) && (numBytesSent == 0))) {
      CloseSocket ();
    }
  } catch (std::exception& e) {
    std::cerr << "Error (socket): " << e.what () << std::endl;
    return false;
  }
  return true;
}

bool Socket::Recv (void* buffer, int numBytes)
{
  while (IsNotConnected ()) {
    sleep (1);
    ConnectSocket ();
  }

  try {
    int numBytesRead = -1;
    numBytesRead = read (mSocketFD, (char *) buffer, numBytes);

    // if read returns error, or if TCP and read 0 close to reconnect
    if ((numBytesRead == -1) || ((mProtocol == TCP) && (numBytesRead == 0))) {
      CloseSocket ();
    }
  } catch (std::exception& e) {
    std::cerr << "Error (socket): " << e.what () << std::endl;
    return false;
  }
  return true;
}

Socket& Socket::operator= (Socket&& other)
{
  if (this == &other) return *this;
  CloseSocket ();

  mHostname = std::move (other.mHostname);
  mPort = std::move (other.mPort);
  mProtocol = std::move (other.mProtocol);
  mSocketFD = std::move (other.mSocketFD);
  mIsConnected = std::move (other.mIsConnected);
  mIsHostSocket = std::move (other.mIsHostSocket);
  mHostInfo = std::move (other.mHostInfo);

  other.mSocketFD = -1;

  return *this;
}

std::ostream& operator<< (std::ostream& out, const Socket& socket)
{
  out << "Socket (" << (socket.mIsHostSocket ? "" : "host: " + socket.mHostname + ", ")
      << "port: " << socket.mPort << ", protocol: "
      << (socket.mProtocol == simple_socket::Socket::TCP ? "TCP" : "UDP")
      << ", file descriptor: " << socket.mSocketFD << ", connected: "
      << socket.mIsConnected << ", is host: " << socket.mIsHostSocket << ")";
  return out;
}

int Socket::CreateSocket ()
{
  int sockFD = -1;
  switch (mProtocol) {
    case TCP:
      sockFD = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
      break;
    case UDP:
      sockFD = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
      break;
    default:
      break;
  }
  return sockFD;
}

void Socket::CloseSocket ()
{
  close (mSocketFD);
  mSocketFD = -1;
  mIsConnected = false;
}

int Socket::BindSocket (int portNum)
{
  sockaddr_in sockInfo;
  memset ((char *)& sockInfo, 0, sizeof (sockInfo));
  sockInfo.sin_family = AF_INET;
  sockInfo.sin_port = htons (portNum);
  sockInfo.sin_addr.s_addr = htonl (INADDR_ANY);
  return bind (mSocketFD, (sockaddr *) &sockInfo, sizeof (sockInfo));
}

bool Socket::ConnectSocket ()
{
  if (IsConnected ()) return true;

  // socket was closed due to send/recv error, recreate
  if (mSocketFD == -1) {
    mSocketFD = CreateSocket ();
    if (mIsHostSocket) {
      BindSocket (mPort);
    }
  }

  switch (mProtocol) {
    case TCP:
      mIsConnected = mIsHostSocket ?
        ListenAndAccept () :
        (connect (mSocketFD, (sockaddr *)& mHostInfo, sizeof (mHostInfo)) == 0);
      break;
    case UDP:
      mIsConnected = true;
      break;
    default:
      break;
  }

  #ifdef DEBUG
  if (IsNotConnected ()) {
    perror ("Socket::ConnectSocket: failed to connect");
  }
  #endif

  return mIsConnected;
}

bool Socket::ListenAndAccept ()
{
  try {
    if (listen (mSocketFD, 5) == -1) {
      throw std::runtime_error ("listen failed");
    }

    int newSocketFD = accept (mSocketFD, nullptr, nullptr);
    if (newSocketFD == -1) {
      throw std::runtime_error ("accept failed");
    }

    close (mSocketFD);
    mSocketFD = newSocketFD;
  } catch (std::exception& e) {
    std::cerr << "Error (socket): " << e.what () << std::endl;
    return false;
  }
  return true;
}

} // namespace simple_socket
