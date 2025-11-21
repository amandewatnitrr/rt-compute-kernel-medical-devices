#include <stdio.h>
#include <assert.h>
#include "task.h"

// No stub needed for scheduler_add_task as we will link the actual object file.

void test_task_creation() {
    printf("Running test_task_creation...\n");
    
    // We are not resetting state between tests, so we can't assert a specific ID.
    int task_id = task_create(NULL, NULL, 1);
    assert(task_id >= 0); // Check for a valid task ID

    tcb_t* task = task_get_by_id(task_id);
    assert(task != NULL);
    assert(task->id == (uint32_t)task_id);
    assert(task->priority == 1);
    assert(task->state == TASK_READY);
    
    printf("test_task_creation PASSED.\n");
}

