#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#include "mysemaphore.h"

#define N 2
#define LOOPS 10

struct my_semaphore sem;

int count = 0;

void section_crique()
{
    // for (int i = 0; i < 10000; i++); // Emulation de la section critique
    count++;
}

void *thread_code(void *arg){
    for (int i = 0; i < 10; i++)
    {
        sem_wait(&sem);
        section_crique();
        sem_post(&sem);
    }
}

int main(int argc, char const *argv[]){
    int err;
    err = sem_init(&sem,1);
    if(err!=0) {
      printf("ERROR sem_init");
    }
    
    pthread_t threads_bloc[N];

    
    for (int i = 0; i < N; i++)
    {
        err = pthread_create(&threads_bloc[i], NULL, thread_code, NULL);
        if(err!=0){
            return 1;
        }
    }

        for (int i = 0; i < N; i++)
    {
        err = pthread_join(threads_bloc[i], NULL);
        if(err!=0){
            return 1;
        }
    }
    sem_destroy(&sem);

    printf("%d\n", count);
    return 0;
}
