# C++ HTTP Server (WSL / POSIX)

A lightweight, single-header-style POSIX/WSL C++ HTTP server implementation using raw sockets. It supports custom request routing, JSON responses via POST endpoints, and static file handling capabilities.

---

## Features

- **POSIX Socket Integration:** Built using standard Linux/WSL socket APIs (`socket`, `bind`, `listen`, `accept`).
- **Endpoint Routing:** Includes map-based route handlers for POST requests (such as `/api/echo` and `/api/reverse`).
- **Static File & MIME Support:** Infrastructure ready for serving files with dynamic MIME type resolution and 404 handling.
- **Port Customization:** Accepts command-line argument configuration with robust fallback and out-of-range port validation.
- **Connection Management:** Reusable socket option (`SO_REUSEADDR`) enabled for quick server restarts.

---

## Prerequisites

To compile and run this server, ensure your system has:
- A C++ compiler supporting C++11 or later (e.g., `g++`)
- A Linux or WSL (Windows Subsystem for Linux) environment

---

## Compilation

Compile the code using `g++` via your terminal:

```bash
g++ -std=c++11 main.cpp -o http_server