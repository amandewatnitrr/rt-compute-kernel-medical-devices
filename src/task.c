#include "task.h"
#include "scheduler.h"
#include <string.h>

static tcb_t tasks[MAX_TASKS];
static uint32_t next_task_id = 0;
static tcb_t* current_task = NULL;

int task_create(task_func_t task_func, void* arg, uint8_t priority) {
    if (next_task_id >= MAX_TASKS) {
        return -1; // Max tasks reached
    }

    tcb_t* new_task = &tasks[next_task_id];
    memset(new_task, 0, sizeof(tcb_t));

    new_task->id = next_task_id;
    new_task->state = TASK_READY;
    new_task->priority = priority;
    new_task->task_func = task_func;
    new_task->arg = arg;
    // In a real kernel, the stack pointer would be initialized here.
    new_task->sp = &new_task->stack[TASK_STACK_SIZE - 1];

    scheduler_add_task(new_task);

    next_task_id++;
    return new_task->id;
}

tcb_t* task_get_current() {
    return current_task;
}

void task_set_current(tcb_t* task) {
    current_task = task;
}

tcb_t* task_get_by_id(uint32_t id) {
    if (id < next_task_id) {
        return &tasks[id];
    }
    return NULL;
}

// This is an internal function for the scheduler to get all tasks
tcb_t* task_get_all_tasks() {
    return tasks;
}

uint32_t task_get_count() {
    return next_task_id;
}
