# Secure Operating System Simulator

A C++ educational simulator for exploring operating-system scheduling, deadlock avoidance, memory management, and inter-process communication. The project was developed for the CS-303 Operating Systems course at HITEC University.

## Features

- Hybrid CPU scheduling using Round Robin and SRTF concepts
- Deadlock avoidance with Banker's Algorithm
- Best-fit memory allocation
- LRU paging
- IPC examples using shared memory, semaphores, and message passing
- Performance indicators such as waiting time, turnaround time, throughput, and CPU utilization

## Build and Run

The project includes source files under `src/` and headers under `include/`. When the repository's source layout and build script are available, a typical Linux build is:

```bash
g++ -std=c++17 -pthread src/*.cpp -Iinclude -o os_sim
./os_sim
```

Adjust the command if the repository's build files use a different entry point.

## Project Structure

```text
src/             # C++ source files
include/         # Header files
Report/          # Project report, if present
screenshots/     # Output screenshots, if present
sim_results.txt  # Recorded results, if present
README.md        # Project documentation
```

## Contributors

- Hassan Ali
- Muneeb Ahmed
- Shayan Latif
- Syed Haider Abbas

## Learning Outcomes

The simulator connects operating-system theory with executable examples of scheduling, resource allocation, memory policies, and IPC.