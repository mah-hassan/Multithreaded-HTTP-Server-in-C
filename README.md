# 🧵 Multithreaded HTTP Server in C

A lightweight, efficient HTTP server built from scratch in C, featuring a **thread pool**, **custom request parsing**, and **queue-based connection handling**. Designed to explore low-level networking and concurrency in C.

## 🚀 Goal

The goal of this project is to:

- Understand how HTTP servers work at the socket level.
- Implement a thread pool and manage concurrency manually using POSIX threads.
- Practice using synchronization primitives like mutexes and condition variables.
- Build a performance-efficient server that can handle many concurrent connections.

## ✅ Design Evolution

This project was built incrementally:

1. **Single-threaded server**: Initially accepted and handled one request at a time.
2. **Thread-per-request**: Spawned a new thread for every connection. This led to high CPU usage and system instability under heavy load.
3. **Thread pool implementation**:
   - Fixed number of worker threads created on startup.
   - Main thread accepts incoming connections and enqueues them.
   - Worker threads dequeue and process requests.
   - Used `pthread_mutex_t` for mutual exclusion and `pthread_cond_t` for efficient thread blocking/wakeup.

This final design ensures **low CPU usage**, **predictable memory usage**, and **high throughput**.

## 📂 Project Structure

```
.
├── src/
│   ├── main.c               # Entry point and server loop
│   └── lib/
│       ├── request.c/.h     # HTTP request parsing
│       ├── response.c/.h    # Response building and sending
│       ├── queue.c/.h       # Thread-safe queue implementation
├── tests/                   # Load testing scripts using k6
├── public/                  # Static assets (index.html)
├── Makefile
└── README.md
```

## 🧠 Build & Run

### Prerequisites

- GCC compiler
- `make`

### Compile

```bash
make
```

### Run the Server

```bash
./server
```

Server will listen on port `3000`.

## 🧪 Load Testing

### Tool: [k6](https://k6.io/)

### Example Test Script (`tests/test.js`)

```javascript
import http from 'k6/http';
import { check } from 'k6';

export const options = {
  stages: [
    { duration: '10s', target: 50 },
    { duration: '10s', target: 100 },
    { duration: '10s', target: 200 },
  ],
};

export default function () {
  const res = http.get('http://localhost:3000/');
  check(res, {
    'status is 200': (r) => r.status === 200,
  });
}
```

### Run the test:

```bash
k6 run tests/test.js
```

## ⚙️ Thread Pool Design

- A fixed number of worker threads are created at startup.
- Each thread waits for incoming connections using `pthread_cond_wait()`.
- The main thread accepts connections and pushes them into a thread-safe queue.
- Worker threads dequeue and handle requests concurrently.

## 📊 Performance Insights

- Efficient under heavy load due to pre-spawned threads.
- Condition variables avoid busy-waiting, keeping CPU usage low.
- Supports thousands of concurrent HTTP requests with predictable latencies.

---

This project provides foundational understanding for building high-performance network servers in C, while reinforcing key concepts in multithreading, synchronization, and systems programming.

