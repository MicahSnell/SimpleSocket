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
    mSocketFD (-1),
    mIsConnected (false),
    mIsHostSocket (mHostname.empty ())
{
  try {
    switch (mProtocol) {
    case TCP:
      mSocketFD = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
      BindSocket (mPort);
      break;
    case UDP:
      mSocketFD = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
      mIsConnected = (BindSocket (mPort) == 0);
      break;
    default:
      throw std::runtime_error ("Socket: unknown connection type");
      break;
    }

    if (mSocketFD == -1) {
      throw std::runtime_error ("Socket: failed to get file descriptor");
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
    mSocketFD (-1),
    mIsConnected (false),
    mIsHostSocket (mHostname.empty ())
{
  try {
    switch (mProtocol) {
    case TCP:
      mSocketFD = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
      break;
    case UDP:
      mSocketFD = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
      mIsConnected = true;
      break;
    default:
      throw std::runtime_error ("Socket: unknown connection type");
      break;
    }

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
  close (mSocketFD);
}

/**
 * @brief attempts to connect socket to client/host
 * @return true if successful
 */
bool Socket::ConnectSocket ()
{
  switch (mProtocol) {
  case TCP:
    if (mIsHostSocket) {
      mIsConnected = ListenAndAccept ();
    } else {
      mIsConnected = (connect (mSocketFD, (sockaddr *) &mHostInfo, sizeof (mHostInfo)) == 0);
    }
    break;
  case UDP:
    mIsConnected = true;
    break;
  default:
    std::cerr << "Socket::ConnectSocket: unknown protocol, doing nothing\n";
    break;
  }
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
    switch (mProtocol) {
    case TCP:
      if (send (mSocketFD, (char *) buffer, numBytes, MSG_NOSIGNAL) == -1) {
        std::cerr << "Socket::Send: failed to send\n";
        isSuccess = false;
      }
      break;
    case UDP:
      {
        int numBytesSent =
          sendto (mSocketFD, (char *) buffer, numBytes, 0, (sockaddr *) &mHostInfo,
                  sizeof (mHostInfo ));

        if (numBytesSent == -1) {
          std::cerr << "Socket::Send: failed to send\n";
          isSuccess = false;
        }
      }
      break;
    default:
      std::cerr << "Socket::Send: unknown protocol, doing nothing\n";
      isSuccess = false;
      break;
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
    switch (mProtocol) {
    case TCP:
    case UDP:
      if (read (mSocketFD, (char *) buffer, numBytes) == -1) {
        std::cerr << "Socket::Recv: failed to read\n";
        isSuccess = false;
      }
      break;
    default:
      std::cerr << "Socket::Recv: unknown protocol, doing nothing\n";
      isSuccess = false;
      break;
    }
  } catch (std::exception &except) {
    std::cerr << __FILE__ << ":" << __LINE__ << " caught exception: " << except.what ()
              << std::endl;
    isSuccess = false;
  }
  return isSuccess;
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
 * @brief sets options for socketFD based on protocol
 * @param socketFD the file descriptor for the socket
 */
void Socket::SetSocketOptions (int socketFD)
{
  switch (mProtocol) {
  case TCP:
    break;
  case UDP:
    break;
  default:
    std::cerr << "Socket::setSocketOptions: unknown protocol, doing nothing\n";
    break;
  }
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
