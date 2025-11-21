#include "kernel.h"
#include "task.h"
#include "timer.h"
#include <stdio.h>
#include <unistd.h> // For usleep

// Shared data representing medical device readings
typedef struct {
    int heart_rate;
    int drug_delivery_status;
} medical_data_t;

medical_data_t shared_medical_data;
mutex_t data_mutex;

// --- Task Definitions ---

// High-priority task: Monitors vital signs
void task_monitor_vitals(void* arg) {
    (void)arg;
    printf("{\"type\": \"log_entry\", \"message\": \"VitalSigns: Task started.\"}\n");
    
    mutex_lock(&data_mutex);
    shared_medical_data.heart_rate = 70 + (timer_get_ticks() % 15) - 5; // Simulate reading
    printf("{\"type\": \"data\", \"tick\": %d, \"hr\": %d}\n", timer_get_ticks(), shared_medical_data.heart_rate);
    printf("{\"type\": \"log_entry\", \"message\": \"VitalSigns: Heart rate is %d bpm.\"}\n", shared_medical_data.heart_rate);
    mutex_unlock(&data_mutex);

    kernel_sleep(2); // Run every 2 ticks
}

// Medium-priority task: Administers medication
void task_deliver_drug(void* arg) {
    (void)arg;
    printf("{\"type\": \"log_entry\", \"message\": \"DrugDelivery: Task started.\"}\n");

    mutex_lock(&data_mutex);
    shared_medical_data.drug_delivery_status = 1; // Administering
    printf("{\"type\": \"log_entry\", \"message\": \"DrugDelivery: Administering medication.\"}\n");
    mutex_unlock(&data_mutex);

    kernel_sleep(5); // Run every 5 ticks
}

// Low-priority task: Updates the display
void task_update_display(void* arg) {
    (void)arg;
    printf("{\"type\": \"log_entry\", \"message\": \"Display: Task started.\"}\n");

    mutex_lock(&data_mutex);
    printf("{\"type\": \"log_entry\", \"message\": \"Display: Updating screen - HR: %d, Drug Status: %d\"}\n", 
           shared_medical_data.heart_rate, shared_medical_data.drug_delivery_status);
    mutex_unlock(&data_mutex);

    kernel_sleep(3); // Run every 3 ticks
}


// --- Main Application ---

int main() {
    // Set stdout to be unbuffered
    setvbuf(stdout, NULL, _IONBF, 0);

    printf("{\"type\": \"log_entry\", \"message\": \"Medical Device Kernel Demo\"}\n");
    printf("{\"type\": \"log_entry\", \"message\": \"--------------------------\"}\n");

    // Initialize shared data and mutex
    shared_medical_data.heart_rate = 0;
    shared_medical_data.drug_delivery_status = 0;
    mutex_init(&data_mutex);

    // Initialize the kernel
    kernel_init();

    // Create tasks
    task_create(task_monitor_vitals, NULL, 3);  // Highest priority
    task_create(task_deliver_drug, NULL, 2);    // Medium priority
    task_create(task_update_display, NULL, 1);  // Lowest priority

    // Start the kernel
    // This function will not return.
    kernel_start();

    return 0;
}
