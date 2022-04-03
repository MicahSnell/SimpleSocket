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

/**
 * @brief initializes a host socket
 * @param portNum the port number the socket will bind to
 * @param protocol the connection type of the socket
 */
Socket::Socket (int portNum, eProtocol protocol)
  : mPort (portNum),
    mProtocol (protocol),
    mSocketFD (CreateSocket ()),
    mIsConnected (false),
    mIsHostSocket (mHostname.empty ())
{
  try {
    if (mSocketFD == -1) {
      throw std::runtime_error ("Socket: failed to get file descriptor");
    }

    switch (mProtocol) {
    case TCP:
      BindSocket (mPort);
      mIsConnected = false;
      break;
    case UDP:
      mIsConnected = (BindSocket (mPort) == 0);
      break;
    default:
      throw std::runtime_error ("Socket: unknown protocol");
      break;
    }

    #ifdef DEBUG
    std::cout << "Socket: created host socket, is connected: "
              << std::boolalpha << mIsConnected << std::endl;
    #endif
  } catch (std::exception &except) {
    std::cerr << __FILE__ << ":" << __LINE__ << " caught exception: " << except.what ()
              << std::endl;
    throw;
  }
}

/**
 * @brief initializes a client socket
 * @param hostStr the hostname or ip to connect to
 * @param portNum the port number on the host to connect to
 * @param protocol the connection type of the socket
 */
Socket::Socket (std::string hostStr, int portNum, eProtocol protocol)
  : mHostname (hostStr),
    mPort (portNum),
    mProtocol (protocol),
    mSocketFD (CreateSocket ()),
    mIsConnected (mProtocol == UDP),
    mIsHostSocket (mHostname.empty ())
{
  try {
    if (mSocketFD == -1) {
      throw std::runtime_error ("Socket: failed to get file descriptor");
    }

    // get host info and set in sockaddr_in struct
    hostent *hostEntry = gethostbyname (mHostname.c_str ());
    if (hostEntry == nullptr) {
      throw std::runtime_error ("Socket: error getting host info for " + mHostname);
    }
    memset ((char *) &mHostInfo, 0, sizeof (mHostInfo));
    mHostInfo.sin_family = AF_INET;
    mHostInfo.sin_port = htons (mPort);
    mHostInfo.sin_addr = *(in_addr *) hostEntry->h_addr;

    #ifdef DEBUG
    std::cout << "Socket: created client socket, is connected: "
              << std::boolalpha << mIsConnected << std::endl;
    #endif
  } catch (std::exception &except) {
    std::cerr << __FILE__ << ":" << __LINE__ << " caught exception: " << except.what ()
              << std::endl;
    throw;
  }
}

/**
 * @brief closes any open file descriptors
 */
Socket::~Socket ()
{
  CloseSocket ();
}

/**
 * @brief attempts to connect socket to client/host
 * @return true if successful
 */
bool Socket::ConnectSocket ()
{
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
      connect (mSocketFD, (sockaddr *) &mHostInfo, sizeof (mHostInfo)) == 0;
    break;
  case UDP:
    mIsConnected = true;
    break;
  default:
    std::cerr << "Socket::ConnectSocket: unknown protocol, doing nothing\n";
    break;
  }

  #ifdef DEBUG
  if (IsNotConnected ()) {
    perror ("Socket::ConnectSocket: failed to connect");
  }
  #endif

  return mIsConnected;
}

/**
 * @brief attempts to send data given in buffer
 * @param buffer a pointer to the data to be sent
 * @param numBytes the number of bytes to be sent
 * @return true if data is sent successfully
 */
bool Socket::Send (const void *buffer, int numBytes)
{
  bool isSuccess = true;
  try {
    int numBytesSent = -1;
    switch (mProtocol) {
    case TCP:
      numBytesSent = send (mSocketFD, (char *) buffer, numBytes, MSG_NOSIGNAL);
      break;
    case UDP:
      numBytesSent = sendto (mSocketFD, (char *) buffer, numBytes, 0, (sockaddr *) &mHostInfo,
                             sizeof (mHostInfo ));
      break;
    default:
      std::cerr << "Socket::Send: unknown protocol, doing nothing\n";
      isSuccess = false;
      break;
    }

    // if send returns error, of if TCP and sent 0, close to reconnect
    if ((numBytesSent == -1) || ((mProtocol == TCP) && (numBytesSent == 0))) {
      std::cerr << "Socket::Send: failed to send\n";
      CloseSocket ();
      isSuccess = false;
    }
  } catch (std::exception &except) {
    std::cerr << __FILE__ << ":" << __LINE__ << " caught exception: " << except.what ()
              << std::endl;
    isSuccess = false;
  }
  return isSuccess;
}

/**
 * @brief attempts to read data into given buffer
 * @param buffer a pointer to where data should be read to
 * @param numBytes the maximum number of bytes that can be read into @a buffer
 * @return true if successful
 */
bool Socket::Recv (void *buffer, int numBytes)
{
  bool isSuccess = true;
  try {
    int numBytesRead = -1;
    switch (mProtocol) {
    case TCP:
    case UDP:
      numBytesRead = read (mSocketFD, (char *) buffer, numBytes);
      break;
    default:
      std::cerr << "Socket::Recv: unknown protocol, doing nothing\n";
      isSuccess = false;
      break;
    }

    // if read returns error, or if TCP and read 0 close to reconnect
    if ((numBytesRead == -1) || ((mProtocol == TCP) && (numBytesRead == 0))) {
      std::cerr << "Socket::Recv: failed to read\n";
      CloseSocket ();
      isSuccess = false;
    }
  } catch (std::exception &except) {
    std::cerr << __FILE__ << ":" << __LINE__ << " caught exception: " << except.what ()
              << std::endl;
    isSuccess = false;
  }
  return isSuccess;
}

/**
 * @brief creates a socket based on the mProtocol
 * @return integer of the file descriptor for the created socket
 */
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
    std::cerr << "Socket::CreateSocket: unknown protocol\n";
    break;
  }
  return sockFD;
}

/**
 * @brief binds mSocketFD to portNum
 * @param portNum the port number to bind the socket to
 * @return return code of bind call, 0 if successful
 */
int Socket::BindSocket (int portNum)
{
  sockaddr_in sockInfo;
  memset ((char *) &sockInfo, 0, sizeof (sockInfo));
  sockInfo.sin_family = AF_INET;
  sockInfo.sin_port = htons (portNum);
  sockInfo.sin_addr.s_addr = htonl (INADDR_ANY);
  return bind (mSocketFD, (sockaddr *) &sockInfo, sizeof (sockInfo));
}

/**
 * @brief closes the socket and sets as not connected
 */
void Socket::CloseSocket ()
{
  close (mSocketFD);
  mSocketFD = -1;
  mIsConnected = false;
}

/**
 * @brief listens and accepts TCP connections
 * @return true if successfully connected
 */
bool Socket::ListenAndAccept ()
{
  if (listen (mSocketFD, 5) == -1) {
    std::cerr << "Socket::ListenAndAccept: listen failed\n";
    return false;
  }

  int newSocketFD = accept (mSocketFD, nullptr, nullptr);
  if (newSocketFD == -1) {
    std::cerr << "Socket::ListenAndAccept: accept failed\n";
    return false;
  }

  close (mSocketFD);
  mSocketFD = newSocketFD;
  return true;
}
