
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_CYCLES 10000000
#define NUM_PHILOSOPHERS 5

pthread_mutex_t *forks;
int num_philosophers;


void lock_forks(int first_fork, int second_fork) {
    pthread_mutex_lock(&forks[first_fork]);
    pthread_mutex_lock(&forks[second_fork]);
}

void unlock_forks(int first_fork, int second_fork) {
    pthread_mutex_unlock(&forks[second_fork]);
    pthread_mutex_unlock(&forks[first_fork]);
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
    // num_philosophers = NUM_PHILOSOPHERS;

    pthread_t threads[num_philosophers];
    int philosopher_numbers[num_philosophers];

    forks = malloc(num_philosophers * sizeof(pthread_mutex_t));
    
    if (!forks) {
        exit(EXIT_FAILURE);
    }

    int err;

    // Initialiser les mutex
    for (int i = 0; i < num_philosophers; i++) {
        err = pthread_mutex_init(&forks[i], NULL);
        if(err!=0){
            return 1;
        }
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

    // Nettoyer les mutex
    for (int i = 0; i < num_philosophers; i++) {
        err = pthread_mutex_destroy(&forks[i]);
        if(err!=0){
            return 1;
        }
    }

    free(forks);
    return 0;
}
