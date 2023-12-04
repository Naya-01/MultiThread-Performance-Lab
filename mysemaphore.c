#include <stdio.h>
#include <stdlib.h>

#include "mysemaphore.h"
#include "verrou.h"

volatile int shared_lock = 0;

int sem_init(struct my_semaphore *sem, int init_value){
    sem = (struct my_semaphore *) malloc(sizeof(struct my_semaphore));
    if(sem == NULL) return -1;
    sem->val = init_value; 
    return 0;
}

int sem_destroy(struct my_semaphore *sem){
    free(sem);
}

int sem_wait(struct my_semaphore *sem){
    // en attendant que le semaphore soit positif, le thread va faire une 
    // attente active
    // while (sem->val<0){}; 
    lock(&shared_lock);
    sem->val=sem->val-1;
    unlock(&shared_lock);
    // UNLOCK()
    return 0;
}

int sem_post(struct my_semaphore *sem){
    if(sem->val>0){
        lock(&shared_lock);
        sem->val=sem->val+1;
        unlock(&shared_lock);
        return 0;
    }
    return -1; 
}