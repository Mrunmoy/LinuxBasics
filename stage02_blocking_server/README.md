# Stage 02 — Blocking Socket Server

## Goal

Create a basic TCP server that:
- Listens for a single client
- Sends a welcome message
- Receives a message from the client

## Key Concepts

- `socket()`, `bind()`, `listen()`, `accept()`, `connect()`
- `read()` / `write()` using socket FDs
- `sockaddr_in` setup for IPv4 localhost

## How to Run

Open **two terminals**:

### Terminal A: Run the server
```bash
./build.sh
./build/stage02_blocking_server/SimpleServer
```

Terminal B: Run the client
```
./build/stage02_blocking_server/SimpleClient
```

## Sample Output

### Server:
```
TCPServer🟢 Server is listening on port 12345...
TCPServer🔌 Client connected!
TCPServer📨 Received: Hi server, this is a C++ client!
TCPServer👋 Client disconnected.
TCPServer🧹 Server socket closed.

```
### Client:
```
TCPClient🔗 Connected to server at 127.0.0.1:12345
TCPClient📥 Server says: Hello from C++ server!
TCPClient🧹 Client socket closed.

```

## Notes
- This is a blocking server — it only handles one client at a time.
- In the next stage, we’ll handle multiple clients simultaneously using select()!