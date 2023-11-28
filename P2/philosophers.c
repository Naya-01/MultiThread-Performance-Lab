
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_CYCLES 10000000
#define NUM_PHILOSOPHERS 5

pthread_mutex_t *forks;
int num_philosophers;


void lock_forks(int first_fork, int second_fork) {
    if(pthread_mutex_lock(&forks[first_fork]) != 0){
        fprintf(stderr, "Error locking  first fork\\n");
        exit(EXIT_FAILURE);
    }
    if(pthread_mutex_lock(&forks[second_fork]) != 0){
        fprintf(stderr, "Error locking  second fork\\n");
        exit(EXIT_FAILURE); 
    }
}

void unlock_forks(int first_fork, int second_fork) {
    if(pthread_mutex_unlock(&forks[second_fork]) != 0){
        fprintf(stderr, "Error unlocking  second fork\\n");
        exit(EXIT_FAILURE);
    }

    if(pthread_mutex_unlock(&forks[first_fork]) != 0){
        fprintf(stderr, "Error unlocking  first fork\\n");
        exit(EXIT_FAILURE);
    }
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
        printf("Philosophe %d pense\n", id);
        lock_forks(first_fork, second_fork);
        printf("Philosophe %d commence à manger\n", id);

        printf("Philosophe %d a fini de manger et repose les fourchettes\n", id);
        unlock_forks(first_fork, second_fork);

    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        return 1;
    }

    // num_philosophers = atoi(argv[1]);
    num_philosophers = NUM_PHILOSOPHERS;

    pthread_t threads[num_philosophers];
    int philosopher_numbers[num_philosophers];

    forks = malloc(num_philosophers * sizeof(pthread_mutex_t));
    
    if (!forks) {
        fprintf(stderr, "Failed to allocate memory for forks \n");
        exit(EXIT_FAILURE);
    }

    int err;

    // Initialiser les mutex
    for (int i = 0; i < num_philosophers; i++) {
        err = pthread_mutex_init(&forks[i], NULL);
        if(err!=0){
            perror("pthread_mutex_init");
            return 1;
        }
    }

    // Créer les threads
    for (int i = 0; i < num_philosophers; i++) {
        philosopher_numbers[i] = i;
        err = pthread_create(&threads[i], NULL, philosopher, &philosopher_numbers[i]);
        if(err!=0){
            perror("pthread_create");
            return 1;
        }
    }

    // Attendre les threads
    for (int i = 0; i < num_philosophers; i++) {
        err = pthread_join(threads[i], NULL);
        if(err!=0){
            perror("pthread_join");
            return 1;
        }
        
    }

    // Nettoyer les mutex
    for (int i = 0; i < num_philosophers; i++) {
        err = pthread_mutex_destroy(&forks[i]);
        if(err!=0){
            perror("pthread_mutex_destroy");
            return 1;
        }
    }

    free(forks);
    return 0;
}
