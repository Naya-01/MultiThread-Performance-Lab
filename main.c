
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_CYCLES 10000000
#define NUM_PHILOSOPHERS 5

pthread_mutex_t *forks;
int num_philosophers;

void* philosopher(void* num) {
    int id = *(int*)num;

    for (int i = 0; i < NUM_CYCLES; i++) {
        printf("Philosophe %d pense\n", id);

        pthread_mutex_lock(&forks[id]);
        pthread_mutex_lock(&forks[(id + 1) % num_philosophers]);
        printf("Philosophe %d commence à manger\n", id);

        printf("Philosophe %d a fini de manger et repose les fourchettes\n", id);
        pthread_mutex_unlock(&forks[(id + 1) % num_philosophers]);
        pthread_mutex_unlock(&forks[id]);

    }
    return NULL;
}

int main(int argc, char *argv[]) {
    // if (argc != 2) {
    //     return 1;
    // }

    //num_philosophers = atoi(argv[1]);
    num_philosophers = NUM_PHILOSOPHERS;

    pthread_t threads[num_philosophers];
    int philosopher_numbers[num_philosophers];

    forks = malloc(num_philosophers * sizeof(pthread_mutex_t));

    // Initialiser les mutex
    for (int i = 0; i < num_philosophers; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }

    // Créer les threads
    for (int i = 0; i < num_philosophers; i++) {
        philosopher_numbers[i] = i;
        pthread_create(&threads[i], NULL, philosopher, &philosopher_numbers[i]);
    }

    // Attendre les threads
    for (int i = 0; i < num_philosophers; i++) {
        pthread_join(threads[i], NULL);
    }

    // Nettoyer les mutex
    for (int i = 0; i < num_philosophers; i++) {
        pthread_mutex_destroy(&forks[i]);
    }

    free(forks);
    return 0;
}
