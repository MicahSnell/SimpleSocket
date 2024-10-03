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
  /** @brief specifies the connection types */
  enum eProtocol {
    TCP,
    UDP,
    MAX_PROTOCOLS
  };

  /**
   * @brief initializes a host socket
   * @param portNum the port number the socket will bind to
   * @param protocol the connection type of the socket
   */
  Socket (int portNum, eProtocol protocol);

  /**
   * @brief initializes a client socket
   * @param hostStr the hostname or ip to connect to
   * @param portNum the port number on the host to connect to
   * @param protocol the connection type of the socket
   */
  Socket (std::string hostStr, int portNum, eProtocol protocol);

  /**
   * @brief move constructs a socket
   * @param other rvalue reference to a socket to construct from
   */
  Socket (Socket&& other);

  /**
   * @brief closes any open file descriptors
   */
  ~Socket ();

  /**
   * @brief attempts to send data given in buffer
   * @param buffer a pointer to the data to be sent
   * @param numBytes the number of bytes to be sent
   * @return true if data is sent successfully
   */
  bool Send (const void* buffer, int numBytes);

  /**
   * @brief attempts to read data into given buffer
   * @param buffer a pointer to where data should be read to
   * @param numBytes the maximum number of bytes that can be read into @a buffer
   * @return true if successful
   */
  bool Recv (void* buffer, int numBytes);

  /** @brief gets if this socket is currently connected */
  bool IsConnected () const
  { return mIsConnected; }

  /** @brief gets if this socket is currently not connected */
  bool IsNotConnected () const
  { return !mIsConnected; }

  /**
   * @brief move assignment operator
   * @param other rvalue reference to a socket to assign from
   */
  Socket& operator= (Socket&& other);

  /**
   * @brief output operator
   * @param socket socket to output
   */
  friend std::ostream& operator<< (std::ostream& out, const Socket& socket);

  // disable copying
  Socket (const Socket&) = delete;
  Socket& operator= (const Socket&) = delete;

private:
  /**
   * @brief creates a socket based on the mProtocol
   * @return integer of the file descriptor for the created socket
   */
  int CreateSocket ();

  /**
   * @brief closes the socket and sets as not connected
   */
  void CloseSocket ();

  /**
   * @brief binds mSocketFD to portNum
   * @param portNum the port number to bind the socket to
   * @return return code of bind call, 0 if successful
   */
  int BindSocket (int portNum);

  /**
   * @brief attempts to connect socket to client/host
   * @return true if successful
   */
  bool ConnectSocket ();

  /**
   * @brief listens and accepts TCP connections
   * @return true if successfully connected
   */
  bool ListenAndAccept ();

  /** host name receiving data */
  std::string mHostname;
  /** holds the port */
  int mPort;
  /** holds the connection type */
  eProtocol mProtocol;
  /** holds the socket file descriptor */
  int mSocketFD;
  /** indicates connection status */
  bool mIsConnected;
  /** indicates if this is a host socket */
  bool mIsHostSocket;
  /** holds host connection info */
  sockaddr_in mHostInfo;
};

} // namespace simple_socket

#endif
