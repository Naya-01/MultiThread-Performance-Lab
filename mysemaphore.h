#ifndef _MYSEMAPHORE_H_
#define _MYSEMAPHORE_H_

typedef struct {
    volatile int val;
    volatile int lock;
} my_semaphore;

int init(my_semaphore *sem, int init_value);

int destroy(my_semaphore *sem);

int wait(my_semaphore *sem);

int post(my_semaphore *sem);


#endif
