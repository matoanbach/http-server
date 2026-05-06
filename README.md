# http-server

Product name: `http-server`.
Repository name: `http-server`.
GitHub repository: [matoanbach/http-server](https://github.com/matoanbach/http-server)

`http-server` is a systems programming project that implements a small HTTP/1.1 server in C++. It is a hands-on exercise in socket programming, request parsing, response construction, file I/O, basic compression, and multi-client handling without relying on a web framework.

## What It Does

This project includes:
- A TCP server that listens on port `4221`.
- Basic HTTP request parsing using string splitting.
- Route handling for a small set of GET and POST endpoints.
- `GET /` returning `200 OK`.
- `GET /echo/{text}` returning the provided text in the response body.
- `GET /user-agent` returning the incoming `User-Agent` header.
- `GET /files/{filename}` reading a file from a configured directory.
- `POST /files/{filename}` writing request-body content into a file and returning `201 Created`.
- Per-request gzip compression for the echo route when the client sends `Accept-Encoding: gzip`.
- Multi-client handling using pthread-created worker threads.

In plain terms, this repo shows how to build a small HTTP server from scratch in C++ instead of using an existing server framework.

## Tech Stack

- C++23
- POSIX sockets
- pthreads
- CMake
- zlib
- vcpkg for dependency management

## Project Layout

- Server entrypoint: `src/server.cpp`
- Core HTTP server logic: `src/http.cpp`, `src/http.h`
- Gzip helpers: `src/gzip.h`
- Build configuration: `CMakeLists.txt`
- vcpkg dependency manifest: `vcpkg.json`, `vcpkg-configuration.json`
- Local build/run helper: `your_program.sh`
- Local build output: `build/`

## Run Locally

Requirements:
- A C++ compiler with C++23 support.
- CMake `3.13+`.
- `zlib` and pthread support.
- `vcpkg` if you want to use the bundled toolchain flow in `your_program.sh`.

Build using the local helper script:

```bash
./your_program.sh
```

Build manually with CMake and vcpkg:

```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake
cmake --build ./build
```

Run the server manually after building:

```bash
./build/server
```

Run the server with a writable file directory:

```bash
./build/server --directory /tmp/http-server-data/
```

The server listens on:

```text
http://localhost:4221
```

## Supported Endpoints

### `GET /`

Returns an empty `200 OK` response.

### `GET /echo/{text}`

Returns `{text}` as `text/plain`.

If the request includes `Accept-Encoding: gzip`, the server returns:
- `Content-Encoding: gzip`
- compressed response data

### `GET /user-agent`

Returns the incoming `User-Agent` header value as `text/plain`.

### `GET /files/{filename}`

Reads a file from the directory supplied through `--directory` and returns it as `application/octet-stream`.

### `POST /files/{filename}`

Writes the parsed request body into a file under the configured directory and returns `201 Created`.

## Example Requests

Health-style request:

```bash
curl -i http://localhost:4221/
```

Echo request:

```bash
curl -i http://localhost:4221/echo/hello
```

Echo request with gzip accepted:

```bash
curl -i -H "Accept-Encoding: gzip" http://localhost:4221/echo/hello
```

User-Agent request:

```bash
curl -i -H "User-Agent: my-client" http://localhost:4221/user-agent
```

Write a file:

```bash
curl -i -X POST http://localhost:4221/files/test.txt --data "hello"
```

Read a file:

```bash
curl -i http://localhost:4221/files/test.txt
```

## How It Works

### Socket And Listener Setup

The server:
- creates a TCP socket
- enables `SO_REUSEADDR`
- binds to `0.0.0.0:4221`
- starts listening with a small connection backlog

This setup lives in `HTTP::run()`.

### Request Handling Model

Each accepted client connection is handled on its own pthread-created worker thread.

The request flow is:

1. Accept a client socket.
2. Read raw bytes into a buffer.
3. Split the request into tokens and headers.
4. Dispatch based on whether the request is parsed as `GET` or `POST`.
5. Build and send a raw HTTP response string.

### Routing Logic

The routing is simple and hand-written:
- `/`
- `/echo/...`
- `/user-agent`
- `/files/...`

This makes the project useful for learning because the routing behavior is explicit in the code rather than hidden behind a framework.

### Compression Behavior

The echo handler checks `Accept-Encoding` and only compresses the response when `gzip` is present.

The gzip helper uses zlib functions from `src/gzip.h`.

### File Handling

When the server is started with `--directory <path>`, it stores that directory path and creates the directory tree if it does not already exist.

The `/files` routes then use that directory as the read/write root.

## Command-Line Usage

Supported runtime flag:
- `--directory <path>`

Behavior:
- if provided, the directory is created if needed
- file read/write routes operate relative to this directory
- if not provided, file routes return `404 Not Found`

## What To Improve

HTTP correctness:
- Replace the hand-written string parsing with a more robust HTTP parser.
- Make request-body parsing respect `Content-Length` fully instead of relying on the final line of the raw request buffer.
- Add better handling for malformed requests and unsupported methods.

File handling:
- Return full file contents safely instead of relying on the current line-based read behavior.
- Support binary-safe file serving more explicitly.
- Add directory traversal protections if this were to evolve beyond a learning project.

Concurrency and reliability:
- Improve thread safety around accepted socket descriptors and shared state.
- Add graceful shutdown behavior and stronger connection lifecycle management.
- Remove debug prints or replace them with structured logs.

Engineering quality:
- Add automated tests for request parsing and endpoint behavior.
- Add CI for building and testing on Linux/macOS.
- Split parsing, routing, and response construction into smaller units for easier maintenance.

Protocol support:
- Add more HTTP methods and richer header handling.
- Extend compression handling beyond the echo route if needed.
- Add keep-alive support, better status handling, and more complete response headers.
