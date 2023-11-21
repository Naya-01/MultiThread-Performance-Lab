
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define BUFFER_SIZE 8
#define TOTAL_ELEMENTS 8192
#define N BUFFER_SIZE
#define NUM_CONSUMERS 5
#define NUM_PRODUCERS 4

int buffer[BUFFER_SIZE];
int produce_ptr = 0;
int consume_ptr = 0;
int produced_count = 0;
int consumed_count = 0;

pthread_mutex_t mutex;
sem_t empty;
sem_t full;

int insert_item(int item) {
    buffer[produce_ptr] = item;
    printf("Producer produced item %d at position %d \n", item, produce_ptr);
    produce_ptr = (produce_ptr + 1) % BUFFER_SIZE;
    return 0;
}

int remove_item(int *item) {
    *item = buffer[consume_ptr];
    printf("Consumer consumed item %d from position %d \n", *item, consume_ptr);
    consume_ptr = (consume_ptr + 1) % BUFFER_SIZE;
    return 0;
}

void* producer(void* arg) {
    int item;
    while (produced_count < TOTAL_ELEMENTS) {
        item = rand(); 

        for (int i = 0; i < 10000; i++);

        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        if (insert_item(item) == -1) {
            fprintf(stderr, "Producer error\n");
        } else {
            produced_count++;
        }

        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
    return NULL;
}

void* consumer(void* arg) {
    int item;
    while (consumed_count < TOTAL_ELEMENTS) {
        for (int i = 0; i < 10000; i++);

        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        if (remove_item(&item) == -1) {
            fprintf(stderr, "Consumer error\n");
        } else {
            consumed_count++;
        }

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    // if (argc != 3) {
    //     fprintf(stderr, "Usage: %s <num_producers> <num_consumers>\n", argv[0]);
    //     return 1;
    // }

    // int num_producers = atoi(argv[1]);
    // int num_consumers = atoi(argv[2]);

    int num_producers = NUM_PRODUCERS;
    int num_consumers = NUM_CONSUMERS;

    pthread_t producers[num_producers];
    pthread_t consumers[num_consumers];

    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, N);
    sem_init(&full, 0, 0);


    int err;

    for (int i = 0; i < num_producers; i++) {
        err = pthread_create(&producers[i], NULL, producer, NULL);
        if(err!=0){
            perror("pthread_create producers");
            return 1;
        }
    }

    for (int i = 0; i < num_consumers; i++) {
        err = pthread_create(&consumers[i], NULL, consumer, NULL);
        if(err!=0){
            perror("pthread_create consumer");
            return 1;
        }
    }

    for (int i = 0; i < num_producers; i++) {
        err = pthread_join(producers[i], NULL);
        if(err!=0){
            perror("pthread_join producer");
            return 1;
        }
    }

    for (int i = 0; i < num_consumers; i++) {
        err = pthread_join(consumers[i], NULL);
        if(err!=0){
            perror("pthread_join consumer");
            return 1;
        }
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}
