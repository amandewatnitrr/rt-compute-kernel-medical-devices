#ifndef KERNEL_H
#define KERNEL_H

#include "task.h"
#include "ipc.h"

// Initializes the kernel, scheduler, and system timer.
void kernel_init();

// Starts the kernel and begins multitasking.
void kernel_start();

// Yields the CPU to another task.
void kernel_yield();

// Puts the current task to sleep for a specified number of ticks.
void kernel_sleep(uint32_t ticks);

#endif // KERNEL_H
