#include "ipc.h"
#include "task.h"
#include "kernel.h"
#include <stdio.h>

void mutex_init(mutex_t* mutex) {
    mutex->locked = 0;
    mutex->owner_task_id = -1;
}

void mutex_lock(mutex_t* mutex) {
    while (mutex->locked) {
        if (mutex->owner_task_id != task_get_current()->id) {
            printf("{\"type\": \"log_entry\", \"message\": \"Task %d: Mutex locked by %d, yielding.\"}\n", task_get_current()->id, mutex->owner_task_id);
            kernel_yield();
        }
    }
    mutex->locked = 1;
    mutex->owner_task_id = task_get_current()->id;
    printf("{\"type\": \"log_entry\", \"message\": \"Task %d: Mutex acquired.\"}\n", task_get_current()->id);
}

void mutex_unlock(mutex_t* mutex) {
    if (mutex->locked && mutex->owner_task_id == task_get_current()->id) {
        mutex->locked = 0;
        mutex->owner_task_id = -1;
        printf("{\"type\": \"log_entry\", \"message\": \"Task %d: Mutex released.\"}\n", task_get_current()->id);
    }
}
