#ifndef _MYSEMAPHORE_H_
#define _MYSEMAPHORE_H_

struct my_semaphore
{
   int val;
};

int sem_init(struct my_semaphore *sem, int init_value);

int sem_destroy(struct my_semaphore *sem);

int sem_wait(struct my_semaphore *sem);

int sem_post(struct my_semaphore *sem);


#endif
