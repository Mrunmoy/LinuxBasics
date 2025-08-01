# Stage 05 — Epoll-based Chat Server

This project demonstrates a simple C++ chat server using the Linux `epoll()` I/O multiplexer.

It builds on earlier stages that used `blocking`, `select()`, and `poll()` I/O models — and introduces efficient, scalable, and modern `epoll()` handling.

---

## What This Server Does

- Accepts multiple client connections
- Uses `epoll()` to monitor sockets for activity
- Gracefully handles client disconnection
- Broadcasts incoming messages to all connected clients (except sender)
- Supports CTRL+C (`SIGINT`) to gracefully shut down the server

---

## Build

From the repo root:

```bash
./build.sh
```

---

## Run

```bash
# Terminal 1 - run the epoll server
./build/stage05_epoll_server/SimpleEpollServer

# Terminal 2+ - connect using clients from stage04 or netcat
./build/stage04_poll_server/SimpleClientForPoll
```

You can also test using:

```bash
telnet 127.0.0.1 12345
```

---

## Implementation Highlights

- Uses `epoll_create1`, `epoll_ctl`, and `epoll_wait`
- Non-blocking I/O on server and client sockets
- Tracks connected clients via a `std::unordered_map`
- Cleans up disconnected sockets immediately on write/read error
- Uses a signal handler (`SIGINT`) for graceful shutdown

---

## Why `epoll()`?

Compared to `select()` and `poll()`, `epoll()`:
- Scales to thousands of sockets
- Avoids linear scanning of file descriptors
- Provides edge-triggered and level-triggered options

---

## Done!

With this stage, you now have:
- A complete working event-driven server
- A solid understanding of a few I/O models in Linux