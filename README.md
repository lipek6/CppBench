# CppBench - System Performance Benchmark
#### Video Demo:  <URL HERE>
#### Description:

**CppBench** is a CLI (Command Line Interface) performance benchmarking tool written in C++. It is designed to stress-test and analyze the primary computing components of a machine: the CPU (Central Processing Unit) and the Disk (Storage I/O).

CppBench utilizes low-level time monitoring and multithreading to provide accurate metrics on execution time, data throughput, and stability (standard deviation).

Besides just running the tests and calling it a day, CppBench lets the user choose how many times the tests shall be executed and automatically calculates the average and deviation of the results. This way, it's possible to have a more precise result and notice the possible existence of system instabilities. 

### Key Features

1.  **CPU Single-Core Stress Test:**
    * Executes a dense arithmetic loop (10 billion operations) pinned to a single thread, measuring the raw sequential processing power of the user's CPU.

2.  **CPU Multi-Core Stress Test:**
    * Automatically detects the machine's available hardware threads (logical cores) and distributes a heavy workload (1 billion operations) across all available cores simultaneously using `std::thread` and vectors to manage concurrency and join results.
    * The thread library used, `std::thread`, allows the code to run on either Linux or Windows without requiring modifications (it just needs to be recompiled).

3.  **Disk I/O Benchmark:**
    * **Write Speed:** Generates a 1GB memory buffer and streams it to a binary file (`teste.bin`) to measure write throughput (MB/s).
    * **Read Speed:** Reads the generated binary file back into memory to measure read throughput.
    * Ensures memory is properly allocated and freed (`malloc`/`free`) to avoid leaks during large transfers.

4.  **Statistical Analysis:**
    * The tool runs multiple test iterations (user-defined).
    * Calculates the **Average** execution time and throughput.
    * Calculates the **Standard Deviation** to determine the stability of the hardware (consistency of the benchmark scores).

### Files and Structure

* `bench.cpp`: The main source code containing all logic, test functions, and the statistical engine.
* `bench.out`: An automatically generated log file that stores detailed results of every test iteration.
* `teste.bin`: A temporary binary file used for disk I/O testing (automatically deleted after execution).

### How to Run

1.  **Compile the code:**
    You must use a C++ compiler. If using `g++` (Linux/Mac/MinGW), use the following command (ensure you link `pthread` for multithreading support):
    ```bash
    g++ -o bench bench.cpp -lpthread
    ```

2.  **Execute the program:**
    ```bash
    ./bench
    ```

3.  **Analyze Results:**
    Follow the on-screen prompts to choose the number of test iterations. Once finished, check the console output or open `bench.out` for a detailed report.

### Technologies Used
* **C++ Standard Library:** `<iostream>`, `<vector>`, `<numeric>`, `<cmath>`
* **Multithreading:** `<thread>` for parallel execution.
* **Time Measurement:** `<chrono>` for high-resolution nanosecond precision.
* **File Handling:** C-style `FILE*` and `fopen` for raw binary I/O performance.

### Note / Curiosity
* This code was originally written in C on my Linux system, but when I shared this GitHub repository with some friends so they could run the benchmark on their machines, the code was unable to run on Windows systems, primarily because of the use of Linux-specific libraries (like `<pthread.h>`).
* Faced with that problem, I decided that the simplest solution to make the code runnable/compilable on both Windows and Linux systems was to port all the code to C++ and use libraries that were able to run on both systems.
* That means that much of the code is in a "C-style," but the migration to the C++ language gave me a little more freedom to write the code easily, such as the use of `std::vector`s and similar features.