#include <stdio.h>
#include <stdlib.h>

#include "mysemaphore.h"
#include "verrou.h"

volatile int shared_lock = 0;

int init(my_semaphore *sem, int init_value) {
    if (sem == NULL) return -1;
    sem->val = init_value;
    sem->lock = 0; 
    return 0;
}

int destroy(my_semaphore *sem){
    free(sem);
}

int wait(my_semaphore *sem){
    lock(&sem->lock);
    if (sem->val > 0)   sem->val--;
    unlock(&sem->lock);
    return 0;
}

int post(my_semaphore *sem){
    if(sem->val <= 0) return -1;
    lock(&sem->lock);
    sem->val++;
    unlock(&sem->lock);
    return 0;
}