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
  if (argc != 4) {
    std::cerr << red << "Wrong number of arguments" << std::endl
              << "Usage: " << argv[0] << " host port tcp/udp" << reset << std::endl
              << std::endl;
    return -1;
  }

  simple_socket::Socket::eProtocol protocol;
  if (strcmp (argv[3], "tcp") == 0) {
    protocol = simple_socket::Socket::TCP;
  } else if (strcmp (argv[3], "udp") == 0) {
    protocol = simple_socket::Socket::UDP;
  } else {
    std::cerr << red << "Unknown protocol, options are tcp or udp" << reset << std::endl
              << std::endl;
    return -1;
  }

  simple_socket::Socket socket (argv[1], atoi (argv[2]), protocol);
  std::vector<int> myData = { 8, 6, 7, 5, 3, 0, 9 };
  std::string myMsg ("Hello World!");

  while (true) {
    // send vector size
    int numBytes = myData.size ();
    if (socket.Send (&numBytes, sizeof (numBytes)) == false) {
      std::cerr << red << "Socket failed to send size\n" << reset;
    }

    // send the vector
    numBytes *= sizeof (int);
    if (socket.Send (&myData[0], numBytes) == false) {
      std::cerr << red << "Socket failed to send vector\n" << reset;
    } else {
      std::cout << "Socket sent values: ";
      for (unsigned i = 0; i < myData.size (); ++i) {
        std::cout << myData[i] << " ";
      }
      std::cout << std::endl;
    }

    // send string size
    numBytes = myMsg.length ();
    if (socket.Send (&numBytes, sizeof (numBytes)) == false) {
      std::cerr << red << "Socket failed to send size\n" << reset;
    }

    // send the string
    if (socket.Send (&myMsg[0], numBytes) == false) {
      std::cerr << red << "Socket failed to send string\n" << reset;
    } else {
      std::cout << "Socket sent string: " + myMsg << std::endl;
    }

    sleep (2);
  }

  return -1;
}
