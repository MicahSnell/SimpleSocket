#include <iostream>
#include <string>
#include <vector>
#include <cstring>

#include <unistd.h>
#include <sys/time.h>

#include "Socket.h"

std::string red ("\033[1;31m");
std::string reset ("\033[0m");

int main (int argc, char *argv[])
{
  if (argc != 3) {
    std::cerr << red << "Wrong number of arguments" << std::endl
              << "Usage: " << argv[0] << " port tcp/udp" << reset << std::endl << std::endl;
    return -1;
  }

  simple_socket::Socket::eProtocol protocol;
  if (strcmp (argv[2], "tcp") == 0) {
    protocol = simple_socket::Socket::TCP;
  } else if (strcmp (argv[2], "udp") == 0) {
    protocol = simple_socket::Socket::UDP;
  } else {
    std::cerr << red << "Unknown protocol, options are tcp or udp" << reset << std::endl << std::endl;
    return -1;
  }

  simple_socket::Socket socket (atoi (argv[1]), protocol);
  std::vector<int> myDataBuffer;
  std::string myMsgBuffer;

  while (true) {
    // recv size of vector
    int numBytes = 0;
    if (socket.Recv (&numBytes, sizeof (numBytes)) == false) {
      std::cerr << red << "Socket failed to recv numBytes\n" << reset;
    }

    // recv in vector
    myDataBuffer.resize (numBytes);
    numBytes *= sizeof (int);
    if (socket.Recv (&myDataBuffer[0], numBytes) == false) {
      std::cerr << red << "Socket failed to recv vector\n" << reset;
    } else {
      std::cout << "Socket recv values: ";
      for (unsigned i = 0; i < myDataBuffer.size (); ++i) {
        std::cout << myDataBuffer[i] << " ";
      }
      std::cout << std::endl;
    }

    // recv size of string
    numBytes = 0;
    if (socket.Recv (&numBytes, sizeof (numBytes)) == false) {
      std::cerr << red << "Socket failed to recv number of bytes\n" << reset;
    }

    // recv the string
    myMsgBuffer.resize (numBytes);
    if (socket.Recv (&myMsgBuffer[0], numBytes) == false) {
      std::cerr << red << "Socket failed to recv string\n" << reset;
    } else {
      std::cout << "Socket recv string: " + myMsgBuffer << std::endl;
    }

    sleep (2);
  }

  return -1;
}
