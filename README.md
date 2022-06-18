# SimpleSocket
An easy to use C++ socket library that supports UDP and TCP connections.

## Build
```bash
$ cd SimpleSocket
$ make
```
Which will create `./build/libSimpleSocket.a` which you can then link in your project as `-lSimpleSocket`

#### Example
To build the example applications:
```bash
$ cd SimpleSocket/example
$ make
```
Which will create `socket_server` and `socket_client`.

#### Debug
All binaries can be built with debug output enabled:
```bash
$ make debug
```

## Minimal Sample
A client socket that sends to localhost on port 54321 using TCP.
```cpp
simple_socket::Socket socket ("localhost", 54321, simple_socket::Socket::TCP);
int myInt = 42;
socket.Send (&myInt, sizeof (myInt));
std::cout << "Sent: " << myInt << std::endl;
```

A host socket that accepts TCP connections on port 54321.
```cpp
simple_socket::Socket socket (54321, simple_socket::Socket::TCP);
int myInt = 0;
socket.Recv (&myInt, sizeof (myInt));
std::cout << "Received: " << myInt << std::endl; // "Received: 42"
```

## Documentation
Documentation is created with `doxygen`:
```bash
$ cd SimpleSocket/doc
$ doxygen Doxyfile
$ <web browser> ./html/index.html
```