#include <stdio.h>

// Forward declarations for test functions
void test_task_creation();
void test_scheduler_initialization();

int main() {
    printf("--- Running All Tests ---\n\n");

    // Run Task tests
    printf("--- Task Tests ---\n");
    test_task_creation();
    printf("------------------\n\n");

    // Run Scheduler tests
    printf("--- Scheduler Tests ---\n");
    test_scheduler_initialization();
    printf("-----------------------\n\n");

    printf("--- All Tests Passed ---\n");

    return 0;
}
