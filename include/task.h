#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include <stdio.h>

#define MAX_TASKS 10
#define TASK_STACK_SIZE 1024

typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED
} task_state_t;

typedef void (*task_func_t)(void*);

typedef struct {
    uint32_t id;
    task_state_t state;
    uint8_t priority;
    uint32_t stack[TASK_STACK_SIZE];
    uint32_t* sp; // Stack pointer
    task_func_t task_func;
    void* arg;
    uint32_t sleep_ticks;
} tcb_t;

// Creates a new task.
int task_create(task_func_t task_func, void* arg, uint8_t priority);

// Gets the currently running task's TCB.
tcb_t* task_get_current();

// Gets the TCB for a given task ID.
tcb_t* task_get_by_id(uint32_t id);

#endif // TASK_H
