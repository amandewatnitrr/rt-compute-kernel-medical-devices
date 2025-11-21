#include "scheduler.h"
#include "timer.h"
#include <stdio.h>

static tcb_t* ready_queue[MAX_TASKS];
static int ready_queue_size = 0;
static int last_scheduled_index = -1;

// Internal function declarations
void task_set_current(tcb_t* task);
tcb_t* task_get_all_tasks();
uint32_t task_get_count();

void scheduler_init() {
    ready_queue_size = 0;
    last_scheduled_index = -1;
}

void scheduler_add_task(tcb_t* task) {
    if (ready_queue_size < MAX_TASKS) {
        ready_queue[ready_queue_size++] = task;
    }
}

void schedule() {
    tcb_t* next_task = NULL;
    int highest_priority = -1;

    // First, find the highest priority among all ready tasks
    for (int i = 0; i < ready_queue_size; i++) {
        if (ready_queue[i]->state == TASK_READY && (int)ready_queue[i]->priority > highest_priority) {
            highest_priority = ready_queue[i]->priority;
        }
    }

    if (highest_priority == -1) {
        printf("{\"type\": \"log_entry\", \"message\": \"Scheduler: No ready tasks. Idling.\"}\n");
        return;
    }

    // Now, find the next ready task at that highest priority, using a round-robin approach
    int start_index = (last_scheduled_index + 1) % ready_queue_size;
    for (int i = 0; i < ready_queue_size; i++) {
        int current_index = (start_index + i) % ready_queue_size;
        if (ready_queue[current_index]->state == TASK_READY && (int)ready_queue[current_index]->priority == highest_priority) {
            next_task = ready_queue[current_index];
            last_scheduled_index = current_index;
            break;
        }
    }

    if (next_task) {
        tcb_t* current_task = task_get_current();
        if (current_task && current_task->state == TASK_RUNNING) {
            current_task->state = TASK_READY;
        }
        
        next_task->state = TASK_RUNNING;
        task_set_current(next_task);

        printf("{\"type\": \"log_entry\", \"message\": \"Scheduler: Running Task %d (Priority: %d)\"}\n", next_task->id, next_task->priority);
        next_task->task_func(next_task->arg);

        if (next_task->state == TASK_RUNNING) {
             next_task->state = TASK_READY;
        }
    } else {
        printf("{\"type\": \"log_entry\", \"message\": \"Scheduler: No ready tasks. Idling.\"}\n");
    }
}

void scheduler_tick() {
    tcb_t* all_tasks = task_get_all_tasks();
    uint32_t num_tasks = task_get_count();
    uint32_t current_ticks = timer_get_ticks();

    for (uint32_t i = 0; i < num_tasks; i++) {
        if (all_tasks[i].state == TASK_BLOCKED && current_ticks >= all_tasks[i].sleep_ticks) {
            all_tasks[i].state = TASK_READY;
        }
    }

    schedule();
}
