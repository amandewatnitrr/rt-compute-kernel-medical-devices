#include "timer.h"
#include <stdio.h>

extern void scheduler_tick();

static uint32_t system_ticks = 0;

void timer_init() {
    system_ticks = 0;
}

void systick_handler() {
    printf("{\"type\": \"log_entry\", \"message\": \"--- Tick %d ---\"}\n", system_ticks);
    system_ticks++;
    
    scheduler_tick();
}

uint32_t timer_get_ticks() {
    return system_ticks;
}
