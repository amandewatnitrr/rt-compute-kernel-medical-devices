#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

// Initializes the system timer.
void timer_init();

// The system timer's interrupt handler.
void systick_handler();

// Gets the current system tick count.
uint32_t timer_get_ticks();

#endif // TIMER_H
