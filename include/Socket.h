/**
 * @file Socket.h
 * definition file for Socket class
 */

#ifndef SOCKET_H__
#define SOCKET_H__

#include <string>

#include <arpa/inet.h>

/** socket interfaces and implementations */
namespace simple_socket {

/**
 * @class Socket a simple client/host udp/tcp connection
 */
class Socket {
public:
  // specifies the connection types
  enum eProtocol {
    TCP,
    UDP,
    MAX_PROTOCOLS
  };
      
  // host socket constructor
  Socket (int portNum, eProtocol protocol);

  // client socket constructor
  Socket (std::string hostStr, int portNum, eProtocol protocol);

  // destructor
  ~Socket ();

  // connect socket
  bool ConnectSocket ();
 
  // send data using socket
  bool Send (const void *buffer, int numBytes);

  // receive data using socket
  bool Recv (void *buffer, int numBytes);

  // gets if this socket is currently connected
  bool IsConnected () const
    { return mIsConnected; }

  // gets if this socket is currently not connected
  bool IsNotConnected () const
    { return !mIsConnected; }

private:
  // creates a socket and stores the file descriptor in mSocketFD
  int CreateSocket ();

  // binds mSocketFD to port
  int BindSocket (int portNum);

  // closes the socket
  void CloseSocket ();

  // listens and accepts TCP connections
  bool ListenAndAccept ();

  // host name receiving data
  std::string mHostname;
  // holds the port
  int mPort;
  // holds the connection type
  eProtocol mProtocol;
  // holds the socket file descriptor
  int mSocketFD;
  // indicates connection status
  bool mIsConnected;
  // indicates if this is a host socket
  bool mIsHostSocket;
  // holds host connection info
  sockaddr_in mHostInfo;
};

} // namespace simple_socket

#endif
