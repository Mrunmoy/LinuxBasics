# Stage 04 — Scalable Chat Server with `poll()`

## Goal

Build a C++ server that:
- Handles many clients concurrently
- Uses the `poll()` API for dynamic FD tracking
- Cleanly shuts down and manages disconnections

## Key Concepts

- `pollfd[]` for socket event monitoring
- Efficient client addition/removal with `std::vector`
- C++ class-based architecture with `TcpServerPoll`
- RAII and signal handling for clean exits

## How to Run

### 1. Build all:
```bash
./build.sh
```

### 2. Run the server:

./build/stage04_poll_server/simple_server

### 3. Connect clients (in separate terminals):

./build/stage04_poll_server/simple_client

## Sample Output

### Server:
```
🟢 poll() server listening on port 12345...
📡 Entering main server loop (poll)...
👥 New client connected: FD = 4
📨 Received from FD 4: Hi server, this is a C++ client!
Client FD 4 disconnected or error.
🧹 Closed and removed client FD 4
^C
🛑 Caught SIGINT. Server will shut down...
poll() failed.
🧹 Server and all client sockets closed.

```
### Client:
```
TCPClient🔗 Connected to server at 127.0.0.1:12345
TCPClient📥 Server says: 👋 Welcome to the poll() chat server!
TCPClient🧹 Client socket closed.
```
