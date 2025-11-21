#include <stdio.h>
#include "scheduler.h"

void test_scheduler_initialization() {
    printf("Running test_scheduler_initialization...\n");
    scheduler_init();
    // This is a simple "smoke test". It passes if it doesn't crash.
    printf("test_scheduler_initialization PASSED.\n");
}

