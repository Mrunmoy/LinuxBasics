# Linux I/O Models in C++ — Progressive Learning Series

This project is a hands-on journey through Linux I/O programming using **C++**, designed to take you from basic file handling to advanced multiplexed servers using `epoll()`.

Each stage builds upon the previous one with real, working code and clear separation of concerns.

---

## 📁 Project Structure

```
epoll/
├── stage01_blocking_io/        # Basic file read/write using Linux syscalls
├── stage02_blocking_server/    # Basic TCP client-server using blocking sockets
├── stage03_select_server/      # Multi-client server using select()
├── stage04_poll_server/        # Improved server using poll()
├── stage05_epoll_server/       # Efficient scalable server using epoll()
```

---

## Stage Summaries

| Stage | Description |
|-------|-------------|
| **01** | Use raw `read()` and `write()` syscalls to write user input to a file |
| **02** | Build a TCP echo server and client using blocking sockets |
| **03** | Introduec `select()` to handle multiple clients concurrently |
| **04** | Improve scalability with `poll()` and cleaner socket management |
| **05** | Finall use `epoll()` for scalable and efficient event-driven server |

---

## How to Build

From the repo root:

```bash
./build.sh
```

This uses CMake to build **all stages** into the `build/` directory.

---

## How to Run a Stage

Each stage produces one or more executables. For example:

```bash
# Run the epoll server
./build/stage05_epoll_server/SimpleEpollServer

# Connect using a simple client
./build/stage04_poll_server/SimpleClientForPoll
```

Or use `telnet`:

```bash
telnet 127.0.0.1 12345
```

---

## Extras

- `DEBUG` flags sprinkled across the code for better traceability
- Signal handling (`SIGINT`) for graceful shutdown
- Client cleanup and broadcasting logic
- C++-ified design for reusability and readability

---

## Status

All stages working and tested locally. Each has its own `README.md` inside its folder.

