*This project has been created as part of the 42 curriculum by nsato.*

<table>
	<thead>
    	<tr>
      		<th style="text-align:center">English</th>
      		<th style="text-align:center"><a href="README_ja.md">Japanese</a></th>
    	</tr>
  	</thead>
</table>

<h1>
	Codexion
</h1> 
<H2>
	Master the race for resources before the deadline masters you
</H2>

## 📖 *Content*
1. [💡 Description](#1-description)
2. [📁 File Structure](#2-file-structure)
3. [✅ Instructions](#3-instructions)
4. [⛏ Additional Requirements](#4-additional-requirements)
5. [📃 Input and Output Examples](#5-input-and-output-examples)
6. [🎁 Bonus](#6-bonus)
7. [🌈 Resources](#7-resources)
	1. [URL](#7-1-url)
	2. [AI Usage](#7-2-ai-usage)
8. [🛡️ Blocking cases handled](#8-blocking-cases-handled)
9. [⚙️ Thread synchronization mechanisms](#9-thread-synchronization-mechanisms)

## 💡 1. Description
Codexion is a concurrency challenge inspired by the Dining Philosophers problem. It simulates a collaborative environment where multiple coders must share limited USB dongles to compile quantum code. The project emphasizes advanced synchronization techniques to prevent deadlocks and starvation, featuring custom schedulers (FIFO and EDF) to ensure fair resource allocation.

## 📁 2. File Structure
- `srcs/main/` : Contains the main entry point and simulation loop.
- `srcs/init/` : Handles argument parsing, data initialization, and resource cleanup.
- `srcs/core/` : Implements the Central Arbiter and the Supervisor routines.
- `srcs/coder/` : Defines the coder threads' logic and state transitions.
- `srcs/heap/` : Custom priority queue implementation for EDF/FIFO scheduling.
- `srcs/utils/` : Utility functions for time management and thread-safe printing.
- `hdrs/` : Header files containing definitions and prototypes.

## ✅ 3. Instructions
### Compilation
To compile the project, simply run:
```bash
make
```

### Execution
The program takes the following mandatory arguments:
```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor [number_of_compiles_required] dongle_cooldown scheduler
```
*Example:*
```bash
./codexion 5 800 200 200 200 7 100 edf
```

## ⛏ 4. Additional Requirements
- **No Global Variables**: State is passed exclusively via pointers.
- **Precision Logging**: State changes must be logged in real-time, and burnout messages must appear within 10ms of the actual burnout.
- **Custom Schedulers**: Must implement `fifo` (First In First Out) and `edf` (Earliest Deadline First) arbitration logic.

## 📃 5. Input and Output Examples
```text
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
200 1 is debugging
400 1 is refactoring
...
1000 3 burned out
```

## 🎁 6. Bonus
*(N/A - This project focuses on the core mandatory features)*

## 🌈 7. Resources
### 7-1. URL
- [POSIX Threads Programming](https://computing.llnl.gov/tutorials/pthreads/)
- [Coffman's Conditions for Deadlock](https://en.wikipedia.org/wiki/Deadlock)

### 7-2. AI Usage
AI was utilized as a pair-programming assistant to:
- Formulate complex priority queue (heap) logic for scheduling.
- Provide theoretical explanations of hardware-level atomic operations and synchronization mechanisms.
- Assist in refactoring code to adhere strictly to the Norminette 25-line limit without losing readability.
- Validate thread-safe logging and debug subtle race conditions (AB-BA deadlocks).

## 🛡️ 8. Blocking cases handled
- **Deadlock Prevention**: Deadlocks are mathematically prevented by breaking Coffman's "Hold and Wait" condition. Coders request both dongles simultaneously through a Central Arbiter. If both are not available, the coder sleeps without holding any dongles, ensuring no circular dependencies form.
- **Starvation Prevention**: Starvation is avoided using the EDF (Earliest Deadline First) and FIFO scheduling policies managed by a priority queue. This guarantees that coders closer to burnout or those who waited longest receive priority.
- **Cooldown Handling**: When dongles are released, their `available_time` is set to `current_time + cooldown`. The Arbiter evaluates this timestamp to enforce strict cooldown periods before reassigning the dongles.
- **Log Serialization**: All console outputs are protected by a dedicated `print_mutex`. Additionally, logging operations check the `is_simulation_running` flag to guarantee that no messages (like "is compiling") are printed after a "burned out" message is issued.

## ⚙️ 9. Thread synchronization mechanisms
This project utilizes a **Central Arbiter Architecture** rather than naive per-dongle mutexes.
- **`scheduler_mutex`**: A single mutex protects the state of all dongles (`AVAILABLE`, `IN_USE`, `COOLDOWN`) and the priority queue. This guarantees atomic evaluation of resource availability and scheduling policies.
- **`pthread_cond_t` (Per-Coder)**: Instead of spin-locking, each coder has a dedicated condition variable. When a coder cannot acquire dongles, it calls `pthread_cond_wait`, efficiently sleeping until another coder releases dongles and issues a `pthread_cond_broadcast`.
- **`time_mutex`**: Protects reads and writes to global timing states such as `last_compile_start` and the `is_simulation_running` flag, preventing race conditions between the coders and the supervisor.
- **Precise Burnout Detection**: The Supervisor thread uses `pthread_cond_timedwait` to sleep exactly until the closest calculated burnout deadline. This avoids CPU-intensive polling while guaranteeing sub-10ms precision for burnout detection.
