#include "timer.h"
#include <stdio.h>

// This would be in a hardware timer's interrupt service routine (ISR)
extern void scheduler_tick();

static uint32_t system_ticks = 0;

void timer_init() {
    system_ticks = 0;
}

// This function simulates a hardware timer interrupt.
void systick_handler() {
    printf("\n--- Tick %d ---\n", system_ticks);
    system_ticks++;
    
    // The scheduler does its time-based work
    scheduler_tick();
}

uint32_t timer_get_ticks() {
    return system_ticks;
}
