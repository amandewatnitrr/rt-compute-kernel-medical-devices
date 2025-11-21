#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "task.h"

// Initializes the scheduler.
void scheduler_init();

// Adds a task to the ready queue.
void scheduler_add_task(tcb_t* task);

// Triggers the scheduler to select the next task to run.
void schedule();

#endif // SCHEDULER_H
