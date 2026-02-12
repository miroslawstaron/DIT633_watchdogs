# DIT633 Watchdog Timer Demonstrations

Code for the DIT633 course at the University of Gothenburg — Lecture 5. Demonstrates the use of watchdog timers in embedded systems.

## Overview

A **watchdog timer** is a common component in embedded systems that detects when the system becomes unresponsive. If the software fails to periodically "kick" (feed) the watchdog within a configured timeout, the watchdog triggers a system reset.

This project contains two demonstrations:

1. **Simple Watchdog** — A single-threaded loop that kicks the watchdog for the first 5 iterations, then deliberately stops kicking to trigger a timeout.
2. **Multi-threaded Watchdog** — Three worker threads run concurrently, each with its own watchdog slot. A dedicated monitor thread checks all watchdogs. Thread 2 simulates a hang after iteration 5, causing the monitor to detect a timeout and initiate a system-wide shutdown.

## Project Structure

| File | Description |
|---|---|
| `main.c` | Entry point — calls the selected watchdog demo |
| `simple_watchdog.c` | Simple single-threaded watchdog implementation and demo |
| `multithreaded_watchdog.c` | Multi-threaded watchdog with per-thread monitoring |
| `watchdog.h` | Shared header with watchdog API declarations |
| `Makefile` | Build configuration |

## Building

```bash
make            # builds the 'lecture5' executable
make clean      # removes object files and the executable
```

## Running

```bash
./lecture5
```

By default, `main.c` runs the multi-threaded demo. To switch to the simple watchdog demo, uncomment the corresponding block in `main.c` and comment out the multi-threaded call.

## Configuration

Key parameters (defined as `#define` constants in the source files):

| Parameter | Default | Description |
|---|---|---|
| `WATCHDOG_TIMEOUT` | 3 s | Time before a watchdog expires |
| `NUM_WORKERS` | 3 | Number of worker threads (multi-threaded demo) |
| `WORKER_ITERATIONS` | 12 | Iterations per worker thread |

## Author

Miroslaw Staron — University of Gothenburg, 2026
