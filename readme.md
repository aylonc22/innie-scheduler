# innie-scheduler  
### Lumon Industries — Innie Task Scheduler (C / Multi-Threaded Edition)

This repository contains **`innie-scheduler`**, a **high-performance, multi-threaded C implementation** of the Lumon Innie Task Scheduler.

It is a **systems-level port and architectural evolution** of the original JavaScript project:  
👉 **[The-Great-Waffle-Machine](https://github.com/aylonc22/The-Great-Waffle-Machine)**

Where the JavaScript version focuses on asynchronous event-loop semantics, this implementation replaces polling and promises with **OS-level threads, condition variables, and explicit deadlock detection**.

---

## Project Motivation

The original scheduler demonstrated deterministic task execution, dependency resolution, and deadlock handling using Node.js and an asynchronous event loop.

This project explores:
- How the same interpreter can be implemented using **POSIX threads**
- How dependency resolution can be handled with **blocking synchronization primitives instead of polling**
- How **hard determinism and low latency** can be achieved without garbage collection

The result is a scheduler that more closely resembles a **real operating-system task manager**.

---

## High-Level Architecture

Each **Innie** is executed as an independent OS thread.

Instead of yielding via `setTimeout`, threads:
- **Block** on condition variables when waiting for dependencies
- **Wake immediately** when a dependency is resolved via a `WAFFLE`

A global registry stores finalized work products and is protected by mutexes to ensure correctness under concurrency.

---

## Core Components

### 1. Innie Execution Model

Each Innie is represented by a struct containing:
- A `pthread_t` handle
- A manual **Program Counter (PC)**
- A local `work_value`
- A private **Shift Stack** for nested loop execution

Each thread runs a worker routine that:
- Parses instructions
- Advances the PC manually
- Blocks or proceeds based on dependency state

---

### 2. Shift Stack (Loop Control)

Nested `SHIFT` blocks are handled via an explicit stack:
- Tracks loop entry points
- Tracks remaining iterations
- Allows precise PC rewinding without recursion

This mirrors how low-level interpreters and virtual machines handle control flow.

---

### 3. Dependency Resolution & Synchronization

- A global **Registry** stores finalized `WAFFLE` values
- Access is protected by `pthread_mutex_t`
- Threads waiting on dependencies sleep using `pthread_cond_t`
- When a `WAFFLE` occurs, waiting threads are **signaled immediately**

This eliminates busy-waiting and reduces CPU usage to zero while blocked.

---

### 4. ANY OF / ALL OF Condition Logic

Condition evaluation mirrors the JavaScript implementation:
- **ANY OF** proceeds when at least one dependency resolves
- **ALL OF** proceeds only when all dependencies resolve
- Short-circuit evaluation prevents unnecessary waiting

The logic is evaluated atomically while holding the registry lock.

---

### 5. Deadlock Detection (Watchdog Thread)

A dedicated **Watchdog Thread** periodically:
- Builds a **directed wait-for graph** of blocked Innies
- Detects cycles using graph traversal
- Triggers the **Deadlock Protocol** on detection

When a deadlock is found:
- All involved Innies are force-released
- Their outputs are set to `-1`
- System progress is guaranteed

This mirrors production-grade deadlock detection used in databases and kernels.

---

## Comparison to the JavaScript Prototype

| Feature | JS Version | C Version |
|------|----------|----------|
| Concurrency Model | Event loop + Promises | OS threads (pthreads) |
| Waiting | Polling (`setTimeout`) | Blocking (`pthread_cond_wait`) |
| CPU Usage While Blocked | Non-zero | Zero |
| Determinism | Best-effort | Strict |
| Deadlock Detection | DFS | Watchdog + wait-for graph |
| Memory Management | Garbage collected | Manual |

Original prototype:  
👉 **[The-Great-Waffle-Machine](https://github.com/aylonc22/The-Great-Waffle-Machine)**

---

## Building & Running

### Requirements
- POSIX-compatible system (Linux / WSL / macOS)
- `gcc` or `clang`
- `pthread`

### Build
```bash
gcc -pthread -o innie-scheduler src/*.c
```

### Run
```bash
./innie-scheduler path/to/schedule.json
```

---

## Design Goals

- Deterministic execution  
- Zero busy-waiting  
- Explicit synchronization  
- Clear separation of concerns  
- Interpreter-style control flow  
- Production-grade deadlock handling  

---

## Future Work

- Lock-free registry optimizations
- Thread pools instead of 1:1 thread mapping
- Persistent distributed registry
- Visualization tooling
- Formal verification of deadlock logic

---

## Reading & References

- Operating Systems: Condition Variables & Mutexes  
- Graph Theory: Cycle Detection in Directed Graphs  
- Interpreter Design: Program Counters & Instruction Dispatch  
- Concurrency Patterns in POSIX Systems  
