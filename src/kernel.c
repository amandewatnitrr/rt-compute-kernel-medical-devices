#include "kernel.h"
#include "scheduler.h"
#include "timer.h"
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

extern void schedule(); // Forward declaration

void kernel_init() {
    printf("{\"type\": \"log_entry\", \"message\": \"Kernel: Initializing...\"}\n");
    scheduler_init();
    timer_init();
    printf("{\"type\": \"log_entry\", \"message\": \"Kernel: Initialization complete.\"}\n");
}

void kernel_start() {
    printf("{\"type\": \"log_entry\", \"message\": \"Kernel: Starting multitasking...\"}\n");
    while (1) {
        systick_handler();
        schedule(); // Run the scheduler
        #ifdef _WIN32
        Sleep(500); // 500ms delay for visualization on Windows
        #else
        usleep(500000); // 0.5s delay for visualization on POSIX
        #endif
    }
}

void kernel_yield() {
    schedule();
}

void kernel_sleep(uint32_t ticks) {
    tcb_t* current_task = task_get_current();
    if (current_task) {
        current_task->state = TASK_BLOCKED;
        current_task->sleep_ticks = timer_get_ticks() + ticks;
        schedule();
    }
}
