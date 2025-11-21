# Deterministic Real-Time Compute Kernel for Medical Devices

## Project Overview

This project is a deterministic, real-time compute kernel designed specifically for the high-reliability and safety-critical environment of medical devices. It provides a preemptive, priority-based scheduling system for managing concurrent tasks, ensuring that high-priority medical functions (like monitoring vital signs or controlling a life-support system) are executed with predictable timing and precision.

The kernel is written in C for performance, portability, and low-level hardware control. It is designed to be lightweight, with a minimal footprint, making it suitable for resource-constrained embedded systems commonly found in medical instrumentation.

### Key Features

- **Deterministic Task Scheduling:** A priority-based, preemptive scheduler ensures that the highest-priority task is always running. It uses round-robin scheduling for tasks of the same priority to ensure fair execution.
- **Task Management:** A simple API for creating, managing, and synchronizing tasks.
- **Synchronization:** Mutexes for safe access to shared resources, preventing data corruption and race conditions.
- **Real-Time Clock:** A system tick that drives the scheduler and allows for precise timing of tasks.
- **Critical Alert Simulation:** Demonstrates how the kernel can identify and flag critical events (e.g., abnormal vital signs) and propagate them to the user interface.
- **Cross-Platform:** Compatible with Windows, macOS, and Linux.
- **Testability:** Includes a suite of unit tests to verify the correctness and reliability of the kernel's components.
- **Extensive Documentation:** Comprehensive documentation to facilitate understanding, use, and certification.

---

## Architecture

The kernel's architecture is modular, with a clear separation of concerns between different components.

![Architecture Diagram](docs/architecture.png)

### Core Components

1. **Task Manager (`task.c`, `task.h`)**
   - Manages the lifecycle of tasks.
   - Uses a Task Control Block (TCB) to store the context of each task, including its ID, priority, state (Ready, Running, Blocked), and stack.

2. **Scheduler (`scheduler.c`, `scheduler.h`)**
   - Implements the core scheduling logic.
   - Maintains a ready list of tasks, sorted by priority.
   - The `schedule()` function selects the next highest-priority task to run.

3. **Kernel (`kernel.c`, `kernel.h`)**
   - Initializes the kernel, task manager, and system timer.
   - Starts the multitasking environment.
   - Provides the main API for interacting with the kernel.

4. **Inter-Process Communication (IPC) (`ipc.c`, `ipc.h`)**
   - Provides synchronization primitives.
   - Currently implements mutexes (`mutex_lock`, `mutex_unlock`) to protect shared resources.

5. **System Timer (`timer.c`, `timer.h`)**
   - Simulates a hardware timer interrupt (`systick_handler`).
   - Triggers the scheduler at regular intervals, enabling preemption and time-based delays.

---

## How to Build and Run

### Prerequisites

- A C compiler (like `gcc`)
- `make` build automation tool

### Building the Project

1. **Clone the repository:**

   ```bash
   git clone https://github.com/amandewatnitrr/idea-hackathon.git
   cd idea-hackathon
   ```

2. **Build the project:**

   From the root directory, run `make`:

   ```bash
   make
   ```

   This will compile the source files and create the main executable `bin/medical_kernel` and a test executable `bin/run_tests`.

### Running the Demo Application

To run the main demo application, execute the following command:

```bash
make run
```

This is a shortcut that will build the project if needed and then run `bin/medical_kernel`.

### Running the Tests

To run the unit tests, execute the following command:

```bash
make test
```

This will build and run the `bin/run_tests` executable, which verifies the functionality of the kernel's components.

---

## Web UI Visualization

This project includes a web-based user interface to visualize the kernel's execution in real-time. It features a live graph of the simulated patient's vital signs, a detailed log of kernel events, and a flashing alert banner for critical events.

### Running the Web UI

1.  **Navigate to the `web` directory:**
    ```bash
    cd web
    ```

2.  **Install dependencies:**
    ```bash
    npm install
    ```

3.  **Start the server:**
    ```bash
    npm start
    ```

4.  **Open your browser:**
    Navigate to `http://localhost:3000` to see the visualization.

---

## Demo Scenario

The main application (`main.c`) simulates a simple medical device with four concurrent tasks:

1. **Vital Signs Monitoring (High Priority)**
   - Represents a critical task that reads and processes a patient's vital signs.
   - Runs frequently and can trigger critical alerts if vitals are abnormal.

2. **Drug Delivery System (Medium Priority)**
   - Represents a task that administers medication at a regular interval.

3. **Display Update (Low Priority, 2 tasks)**
   - Two separate tasks that update a display with the current status or data.
   - These tasks run at the same low priority to demonstrate round-robin scheduling.

The demo showcases the preemptive nature of the scheduler. You will see the high-priority task interrupting the lower-priority ones, and the two display tasks taking turns to run. The mutex is used to safely update a shared data structure containing the medical data.
