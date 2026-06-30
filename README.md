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
	1. [🛡️ Blocking cases handled](#4-1-blocking-cases-handled)
	2. [⚙️ Thread synchronization mechanisms](#4-2thread-synchronization-mechanisms)
5. [🌈 Resources](#5-resources)
	1. [URL](#5-1-url)
	2. [AI Usage](#5-2-ai-usage)

## 💡 1. Description
Master concurrent programming in C through an intense simulation where coders race against burnout while competing for scarce USB dongles.  
Implement POSIX threads, mutexes, condition variables, and sophisticated scheduling algorithms (FIFO/EDF) to orchestrate resource sharing, prevent deadlocks, and ensure fair access—all while keeping your coders productive before the deadline strikes.(from Project PDF)

## 📁 2. File Structure
```
Codexion/
├── Makefile
├── README.md
├── README_ja.md
├── .gitignore
├── hdrs/
│   └── codexion.h
└── srcs/
    ├── main/
    │   ├── main.c
    │   └── simulation.c
    ├── init/
    │   ├── parse.c
    │   ├── init.c
    │   └── init_utils.c
    ├── core/
    │   ├── arbiter.c
    │   └── supervisor.c
    ├── coder/
    ｜   ├── coder.c
    ｜   └── coder_method.c
    ├── heap/
    │   ├── pop_heap.c
    │   ├── push_heap.c
    │   └── heap_utils.c
    └── utils/
         ├── utils.c
         ├── print.c
         └── cleanup.c
```

## ✅ 3. Instructions
1. Compile and create an executable file.  
```bash
make
# ‘codexion’ is created in the root directory.
```
2. When running the program, specify the following arguments.
```
./codexion 1. Number of coders 2. Time until burnout 3. Compilation time 4. Refactoring time 5. Debugging time 6. Number of compilations 7. Dongle cooldown time 8. fifo/edf
```

- *Example:*
```bash
./codexion 4 800 200 200 200 7 100 edf
```
- Delete the generated object files
```bash
make clean
```
- Delete the generated object files and executable files
```bash
make clean
```

- Recompile
```bash
make re
```

## ⛏ 4. Additional Requirements

### 🛡️ 4-1. Blocking cases handled

- Kofman's Conditions  

|Condition Name|Definition/Details|
|----|----|
|Mutual Exclusion|Multiple tasks cannot use a single resource simultaneously; the resource is held by one task at a time.|
|Hold and Wait|A state in which a task is holding at least one resource while another task requests a resource currently in use and waits for it.|
|No Preemption|Another task cannot forcibly take away a resource currently being used by a task.|
|Circular Wait|A situation where the order in which multiple tasks request resources forms a loop.|

- A deadlock can occur when all four of the above conditions are met. In other words, a deadlock can be prevented by breaking any one of these conditions.  
- For example, in Codexion, suppose there are two coders, Coder A and Coder B. If Coder A holds one dongle and Coder B also holds a dongle, neither can compile without the other’s dongle, so they will wait indefinitely for the other’s dongle to become available, resulting in a deadlock.  
- To break this deadlock, we’ve introduced an “Arbiter” to act as a dongle administrator, separate from the coders. When the Arbiter receives a dongle request from a coder, it checks whether the dongles immediately to the left and right of the selected coder—chosen based on the order in a priority queue—are available. If both are available, the Arbiter assigns both to the coder simultaneously; if one or both are unavailable, the Arbiter waits without assigning the dongles to other coders until that coder becomes available. As a result, there are no longer any coders holding a single dongle while waiting, and the hold-and-wait (including circular waiting) cycle is broken, preventing deadlocks from occurring.  
- To prevent starvation, coder requests are managed using a priority queue. If the option is FIFO, dongles are handed out to coders in the order requests are received; if the option is EDF, they are handed out in order of nearest deadline.  
- For accurate burnout detection, the current time is obtained in microseconds when all coders are ready, and the coder routines are executed simultaneously. At this point, a central “Supervisor” is created, which uses `pthread_cond_timedwait` to wait until the time closest to burnout. While waiting, the Supervisor remains ready to receive signals from the coders; if the burnout time is updated, it recalculates the new closest burnout time and repeats the wait. Additionally, calculations and time tracking are performed in microseconds, and converted to milliseconds only at output, to minimize any discrepancy between the output order and the actual order as much as possible.  

### ⚙️ 4-2. Thread synchronization mechanisms
- The mutex and condition variables used in this implementation are as follows:

- pthread_mutex  

|Mutex Variable Name|Variable Being Protected|Used in Which Process|
|----|----|----|
|time_mutex|data->is_simulation_running|Checking and updating whether the main loop continues|
||data->ready_count|Checking whether all coders are at the starting line|
||coder->last_compile_start|Recording the start time of compilation|
||coder->compile_count|Incrementing and checking the number of completed compilations|
||coder->deadline|Stores the coder’s own burnout time|
|scheduler_mutex|data->dogles|Changes the dongle’s state, updates the cooldown time|
||data->wait_queue|Adds to the scheduler’s wait queue|
||coder->in_queue|Pops from the queue|
||coder->request_time|Used for comparison when popping from the queue|
|print_mutex|stdout|Prevents characters from getting mixed up when multiple coders call printf simultaneously|

- pthread_cond_t  

|cond variable name|Waiter|Triggerer|Role|
|----|----|----|----|
|start_cond|All coders|Main thread|Holds all coders in a wait state until they are all created; the main thread then signals them to start the main loop simultaneously|
|exit_cond|Coders currently compiling, debugging, or refactoring|stop_simulation()|If any coder burns out, immediately resumes the waiting coders and terminates the simulation|
|sv_cond|Supervisor|Coders|To prevent the Supervisor from entering an infinite loop or waiting indefinitely due to `usleep`, it waits until the next burnout time|

- To prevent data races caused by multiple threads reading or writing the same variable simultaneously, a corresponding mutex is used for each shared variable.  
- In addition, mutexes are used to protect the minimum necessary critical sections, thereby avoiding race conditions.  
- By using these mutexes and condition variables, shared resources can be accessed safely, ensuring thread safety.  

## 🌈 5. Resources

### 5-1. URL
- [Oracle Help Center Pthreads ライブラリ](https://docs.oracle.com/cd/E19253-01/819-0390/tlib-1/index.html)  
- [Linux Programmer's Manual (7) PTHREADS](https://linuxjm.sourceforge.io/html/LDP_man-pages/man7/pthreads.7.html)  
- [Linux Programmer's Manual (3) PTHREAD_CREATE](https://linuxjm.sourceforge.io/html/LDP_man-pages/man3/pthread_create.3.html)  
- [Linux Programmer's Manual (3) PTHREAD_JOIN](https://linuxjm.sourceforge.io/html/LDP_man-pages/man3/pthread_join.3.html)  
- [Linux Programmer's Manual (3) PTHREAD_MUTEX_*](https://linuxjm.sourceforge.io/html/glibc-linuxthreads/man3/pthread_mutex_init.3.html)  
- [Linux Programmer's Manual (3) PTHREAD_COND_*](https://linuxjm.sourceforge.io/html/glibc-linuxthreads/man3/pthread_cond_init.3.html)  
- [Linux Programmer's Manual (2) GETTIMEOFDAY](https://linuxjm.sourceforge.io/html/LDP_man-pages/man2/gettimeofday.2.html)  
- [Linux Programmer's Manual (3) USLEEP](https://linuxjm.sourceforge.io/html/LDP_man-pages/man3/usleep.3.html)  

### 5-2. AI Usage
- Used gemini/Antigravity  
    - Requirements definition: I asked them to ask questions about my vague concept, which helped solidify the design.  
    - After finalizing the overall design, they broke down the work into tasks to be completed by the target date, and we proceeded according to that plan.  
    - Created a to-do list and daily reports based on the initial design.  
    - For learning and review purposes, I compiled minor questions that arose during the process and saved them in a file.
    - Translation.  