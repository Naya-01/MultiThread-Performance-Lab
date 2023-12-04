#ifndef _MYSEMAPHORE_H_
#define _MYSEMAPHORE_H_

typedef struct {
    volatile int val;
    volatile int lock;
} my_semaphore;

int sem_init(my_semaphore *sem, int init_value);

int sem_destroy(my_semaphore *sem);

int sem_wait(my_semaphore *sem);

int sem_post(my_semaphore *sem);


#endif
