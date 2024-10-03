#include <iostream>
#include <string>
#include <vector>
#include <cstring>

#include <unistd.h>
#include <sys/time.h>

#include "Socket.h"

void usage (const std::string& appName)
{
  std::cout << "Usage: " + appName + " <host> <port> <tcp|udp>" << std::endl;
}

int main (int argc, char** argv)
{
  if (argc != 4) {
    usage (argv[0]);
    return 1;
  }

  simple_socket::Socket::eProtocol protocol;
  if (strcasecmp (argv[3], "tcp") == 0) {
    protocol = simple_socket::Socket::TCP;
  } else if (strcasecmp (argv[3], "udp") == 0) {
    protocol = simple_socket::Socket::UDP;
  } else {
    std::cerr << "Unknown protocol, options are 'tcp' or 'udp'" << std::endl;
    return 1;
  }

  simple_socket::Socket socket (argv[1], atoi (argv[2]), protocol);
  std::vector<int> myValues = { 8, 6, 7, 5, 3, 0, 9 };
  std::string myString ("Hello World!");

  while (true) {
    try {
      // send vector size and values
      int numBytes = myValues.size ();
      if (! socket.Send (&numBytes, sizeof (numBytes))) {
        throw std::runtime_error ("failed to send vector size");
      }

      numBytes *= sizeof (int);
      if (! socket.Send (&myValues[0], numBytes)) {
        throw std::runtime_error ("failed to send vector");
      }

      std::cout << "Sent: ";
      for (unsigned value : myValues) {
        std::cout << value << " ";
      }
      std::cout << std::endl;

      // send string size and values
      numBytes = myString.length ();
      if (! socket.Send (&numBytes, sizeof (numBytes))) {
        throw std::runtime_error ("failed to send string size");
      }

      if (! socket.Send (&myString[0], numBytes)) {
        throw std::runtime_error ("failed to send string");
      }

      std::cout << "Sent: " + myString << std::endl;
    } catch (std::exception& e) {
      std::cerr << "Error (client): " << e.what () << std::endl;
    }
    sleep (2);
  }

  return 1;
}
