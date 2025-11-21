#include "kernel.h"
#include "task.h"
#include "timer.h"
#include <stdio.h>
#include <unistd.h> // For usleep

// Shared data representing medical device readings
typedef struct {
    int heart_rate;
    int spo2;
    int drug_delivery_status;
} medical_data_t;

medical_data_t shared_medical_data;
mutex_t data_mutex;

// --- Task Definitions ---

// High-priority task: Monitors vital signs
void task_monitor_vitals(void* arg) {
    (void)arg;
    static int alert_counter = 0;
    // Approx. 10 minutes in ticks, assuming 1 tick = 0.5s. 10 * 60 / 0.5 = 1200
    const int alert_interval = 1200; 

    printf("{\"type\": \"log_entry\", \"message\": \"VitalSigns: Task started.\"}\n");
    
    mutex_lock(&data_mutex);

    // Simulate occasional critical events
    if (timer_get_ticks() > 0 && (timer_get_ticks() % alert_interval) == 0) {
        shared_medical_data.heart_rate = 150 + (timer_get_ticks() % 10); // Dangerously high HR
        printf("{\"type\": \"alert\", \"message\": \"CRITICAL: Heart Rate is dangerously high!\"}\n");
    } else {
        shared_medical_data.heart_rate = 70 + (timer_get_ticks() % 15) - 5; // Normal HR
    }

    shared_medical_data.spo2 = 95 + (timer_get_ticks() % 5); // Simulate SpO2 (95-99)
    printf("{\"type\": \"data\", \"tick\": %d, \"hr\": %d, \"spo2\": %d}\n", timer_get_ticks(), shared_medical_data.heart_rate, shared_medical_data.spo2);
    printf("{\"type\": \"log_entry\", \"message\": \"VitalSigns: HR=%d bpm, SpO2=%d%%.\"}\n", shared_medical_data.heart_rate, shared_medical_data.spo2);
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
    printf("{\"type\": \"log_entry\", \"message\": \"Display: Updating screen - HR: %d, SpO2: %d, Drug Status: %d\"}\n", 
           shared_medical_data.heart_rate, shared_medical_data.spo2, shared_medical_data.drug_delivery_status);
    mutex_unlock(&data_mutex);

    kernel_sleep(3); // Run every 3 ticks
}

// Another low-priority task to demonstrate round-robin
void task_update_display_2(void* arg) {
    (void)arg;
    printf("{\"type\": \"log_entry\", \"message\": \"Display 2: Task started.\"}\n");
    printf("{\"type\": \"log_entry\", \"message\": \"Display 2: Performing secondary display update.\"}\n");
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
    shared_medical_data.spo2 = 0;
    shared_medical_data.drug_delivery_status = 0;
    mutex_init(&data_mutex);

    // Initialize the kernel
    kernel_init();

    // Create tasks
    task_create(task_monitor_vitals, NULL, 3);  // Highest priority
    task_create(task_deliver_drug, NULL, 2);    // Medium priority
    task_create(task_update_display, NULL, 1);  // Lowest priority
    task_create(task_update_display_2, NULL, 1); // Same lowest priority

    // Start the kernel
    // This function will not return.
    kernel_start();

    return 0;
}
