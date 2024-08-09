# Basic HTTP Server in C++

## Overview
Welcome to the "Build Your Own HTTP Server" project! This project is an educational journey into the world of HTTP, TCP/IP, and server development. You'll find a basic yet functional HTTP server implemented in C++ using CMake. This server is capable of handling simple GET and POST requests, serving static files, and managing multiple concurrent connections.

## Features
- **URL Path Extraction**: Extracts and routes requests based on URL paths. Returns a 200 status for valid paths and 404 for invalid paths.
- **Echo Endpoint**: Implements the `/echo/{str}` endpoint that returns the string provided by the client in the response body.
- **User-Agent Header Reading**: Implements the `/user-agent` endpoint that reads and returns the User-Agent header sent by the client.
- **File Serving**: Implements the `/files/{filename}` endpoint to serve static files and handles file creation via POST requests.
- **Concurrent Connections**: Supports handling multiple concurrent client connections efficiently.
- **Compression Handling**: Parses the `Accept-Encoding` header to determine if the response should be compressed (currently only supports `gzip`).
- **Gzip Compression**: Future implementation planned for compressing responses with `gzip`.

## Getting Started

### Prerequisites
- **C++11 or later**
- **CMake** (version 3.10 or later)
- **GCC/Clang** or any compatible C++ compiler
- **Make** or **Ninja** build system

### Installation
Clone the repository and navigate to the project directory:

```bash
git clone https://github.com/yourusername/http-server-cpp.git
cd http-server-cpp
