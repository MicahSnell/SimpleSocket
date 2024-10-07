#include <iostream>
#include <string>
#include <vector>
#include <cstring>

#include <unistd.h>
#include <sys/time.h>

#include "Socket.h"

void usage (const std::string& appName)
{
  std::cout << "Usage: " + appName + " <port> <tcp|udp>" << std::endl;
}

int main (int argc, char** argv)
{
  if (argc != 3) {
    usage (argv[0]);
    return 1;
  }

  simple_socket::Socket::eProtocol protocol;
  if (strcasecmp (argv[2], "tcp") == 0) {
    protocol = simple_socket::Socket::TCP;
  } else if (strcasecmp (argv[2], "udp") == 0) {
    protocol = simple_socket::Socket::UDP;
  } else {
    usage (argv[0]);
    std::cerr << "Unknown protocol, options are 'tcp' or 'udp'" << std::endl;
    return 1;
  }

  simple_socket::Socket socket (atoi (argv[1]), protocol);
  int msgCount;
  std::vector<int> myValues;
  std::string myString;

  while (true) {
    try {
      // recv message count
      msgCount = -1;
      if (! socket.Recv (&msgCount, sizeof (msgCount))) {
        throw std::runtime_error ("failed to read message count");
      }

      // recv vector size and values
      int numBytes = 0;
      if (! socket.Recv (&numBytes, sizeof (numBytes))) {
        throw std::runtime_error ("failed to read vector size");
      }

      myValues.resize (numBytes);
      numBytes *= sizeof (int);
      if (! socket.Recv (&myValues[0], numBytes)) {
        throw std::runtime_error ("failed to read vector");
      }

      // recv string size and values
      numBytes = 0;
      if (! socket.Recv (&numBytes, sizeof (numBytes))) {
        throw std::runtime_error ("failed to read string size");
      }

      myString.resize (numBytes);
      if (! socket.Recv (&myString[0], numBytes)) {
        throw std::runtime_error ("failed to read string");
      }

      std::cout << "Received (" << msgCount << "):" << std::endl;
      for (unsigned value : myValues) {
        std::cout << value << " ";
      }
      std::cout << myString  << std::endl << std::endl;
    } catch (std::exception& e) {
      std::cerr << "Error (server): " << e.what () << std::endl;
    }
  }

  return 1;
}
