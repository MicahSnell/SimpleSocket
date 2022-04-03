# SimpleSocket
An easy to use C++ socket library that supports UDP and TCP connections.

## Build
```bash
cd SimpleSocket
make
```
Which will create `./lib/libSimpleSocket.a` which you can then link in your project as `-lSimpleSocket`

To build with debug output enabled:
```bash
make debug
```

### Minimal Example
A client socket that sends to localhost on port 54321 using TCP.
```cpp
Socket socket ("localhost", 54321, Socket::TCP);
while (true) {
  while (socket.IsNotConnected ()) {
    sleep (1);
    socket.ConnectSocket ();
  }

  while (socket.IsConnected ()) {
    int myInt = 42;
    socket.Send (&myInt, sizeof (myInt));
  }
}
```

A host socket that accepts TCP connections on port 54321.
```cpp
Socket socket (54321, Socket::TCP);
while (true) {
  while (socket.IsNotConnected ()) {
    sleep (1);
    socket.ConnectSocket ();
  }

  while (socket.IsConnected ()) {
    int myInt = 0;
    socket.Recv (&myInt, sizeof (myInt));
    std::cout << "Received: " << myInt << std::endl; // "Received: 42"
  }
}
```

## Test
To make the test applications:
```bash
cd SimpleSocket/test
make
```
Which will create `socket_server` and `socket_client`.

To build with debug output enabled:
```bash
make debug
```

## Documentation
Documentation is created with `doxygen`:
```bash
cd SimpleSocket/doc
doxygen Doxyfile
<web browser> ./html/index.html
```