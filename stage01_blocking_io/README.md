# Stage 01 — Blocking I/O Echo to File (Syscall Version)

## Goal

Learn the basics of Linux file descriptors and system calls by:
- Reading user input via `read()`
- Writing output via `write()`
- Storing data in a file using `open()` and `write()`

## Key Concepts

- File descriptors (FDs): `0 = stdin`, `1 = stdout`
- System calls: `open()`, `read()`, `write()`, `close()`
- Buffers and manual null-termination (`'\0'`)
- Avoiding C++ iostreams for raw syscall experience

## How to Run

```bash
./build.sh
./build/stage01_blocking_io/echo_to_file
```

## Sample Output
```
Enter text (type quit to exit): This is the first line
This is another
---END---
Enter text (type quit to exit): quit
```
## File Output
```
This is the first line
This is another
```