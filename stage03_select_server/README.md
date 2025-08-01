# Stage 03 — Multi-Client Chat Server with `select()`

## Goal

Build a C++ server that can:
- Accept **multiple client connections**
- Handle all clients **in a single thread** using `select()`
- Broadcast messages from one client to all others

## Key Concepts

- `fd_set`, `FD_SET`, `FD_ZERO`, `select()`
- Watching multiple sockets for activity
- Dynamically managing client connections (`std::set`)
- Using C++ classes (`TcpServerSelect`) for clean architecture

## How It Works

- The server monitors the **server socket** and all **client sockets**
- When a client sends a message, the server:
  - Reads it
  - Broadcasts it to all other clients
- Clients are disconnected cleanly on EOF or error

## How to Run

### 1. Build everything:
```bash
./build.sh
```

### 2. Run the server:
```
./build/stage03_select_server/SimpleSelectServer
```

### 3. In separate terminals, run multiple clients:
```
./build/stage03_select_server/SimpleClientCopy
```

## Sample Output

### Server:
```
🟢 Server is listening on port 12345...
👥 New client connected. FD = 4
📨 From FD 4: Hi server, this is a C++ client!
Client FD 4 disconnected or error.
```
### Client:
```
TCPClient🔗 Connected to server at 127.0.0.1:12345
TCPClient📥 Server says: 👋 Welcome to the chat server!
TCPClient🧹 Client socket closed.
```
## Notes
- Clients currently connect, send one message, then disconnect.
- To create a real chatroom, you'd modify the client to stay connected and read input in a loop.