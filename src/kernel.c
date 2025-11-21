#include "kernel.h"
#include "scheduler.h"
#include "timer.h"
#include <stdio.h>

extern void schedule(); // Forward declaration

void kernel_init() {
    printf("Kernel: Initializing...\n");
    scheduler_init();
    timer_init();
    printf("Kernel: Initialization complete.\n");
}

void kernel_start() {
    printf("Kernel: Starting multitasking...\n");
    // This would normally set up the first task and timer interrupt.
    // For this simulation, we will enter a loop and call the scheduler.
    while (1) {
        // In a real kernel, this loop would be replaced by an idle task
        // and scheduling would be purely interrupt-driven.
        systick_handler();
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
