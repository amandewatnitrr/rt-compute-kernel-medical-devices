#ifndef IPC_H
#define IPC_H

#include <stdint.h>

typedef struct {
    uint8_t locked;
    uint32_t owner_task_id;
} mutex_t;

// Initializes a mutex.
void mutex_init(mutex_t* mutex);

// Locks a mutex, blocking if it's already locked.
void mutex_lock(mutex_t* mutex);

// Unlocks a mutex.
void mutex_unlock(mutex_t* mutex);

#endif // IPC_H
