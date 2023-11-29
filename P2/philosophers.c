#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_CYCLES 10000000
#define NUM_PHILOSOPHERS 5

volatile int *forks;
int num_philosophers;


void lock(int fork) {
    int prev_value;
    do {
        __asm__ __volatile__(
            "xchg %0, %1\n\t"
            : "=r"(prev_value), "+m"(forks[fork])
            : "0"(1)
            :
        );
    } while (prev_value == 1);
}

void unlock(int fork) {
    __asm__ __volatile__(
        "movl $0, %0\n\t"
        : "+m"(forks[fork])
        :
        :
    );
}

void lock_forks(int first_fork, int second_fork) {
    lock(first_fork);
    lock(second_fork);
}

void unlock_forks(int first_fork, int second_fork) {
    unlock(second_fork);
    unlock(first_fork);
}

void* philosopher(void* num) {
    int id = *(int*)num;
    int left_fork = id;
    int right_fork = (id + 1) % num_philosophers;

    int first_fork = right_fork;
    int second_fork = left_fork;

    if (left_fork < right_fork) {
        first_fork = left_fork;
        second_fork = right_fork;
    }

    for (int i = 0; i < NUM_CYCLES; i++) {
        lock_forks(first_fork, second_fork);
        unlock_forks(first_fork, second_fork);

    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        return 1;
    }

    num_philosophers = atoi(argv[1]);
    int philosopher_numbers[num_philosophers];
    pthread_t threads[num_philosophers];
    forks = malloc(num_philosophers * sizeof(int));
    
    if (!forks) {
        exit(EXIT_FAILURE);
    }

    int err;

    // Initialiser les mutex
    for (int i = 0; i < num_philosophers; i++) {
        forks[i] = 0;
    }

    // Créer les threads
    for (int i = 0; i < num_philosophers; i++) {
        philosopher_numbers[i] = i;
        err = pthread_create(&threads[i], NULL, philosopher, &philosopher_numbers[i]);
        if(err!=0){
            return 1;
        }
    }

    // Attendre les threads
    for (int i = 0; i < num_philosophers; i++) {
        err = pthread_join(threads[i], NULL);
        if(err!=0){
            return 1;
        }
        
    }

    free(forks);
    return 0;
}
