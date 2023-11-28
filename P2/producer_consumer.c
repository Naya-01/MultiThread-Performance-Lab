
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define BUFFER_SIZE 8
#define TOTAL_ELEMENTS 8192
#define N BUFFER_SIZE
#define NUM_CONSUMERS 5
#define NUM_PRODUCERS 5

int buffer[BUFFER_SIZE];
int produce_ptr = 0;
int consume_ptr = 0;

volatile int mutex = 0;
sem_t empty;
sem_t full;


void lock() {
    int prev_value;
    do {
        __asm__ __volatile__(
            "xchg %0, %1\n\t"
            : "=r"(prev_value), "+m"(mutex)
            : "0"(1)
            :
        );
    } while (prev_value == 1);
}

void unlock() {
    __asm__ __volatile__(
        "movl $0, %0\n\t"
        : "+m"(mutex)
        :
        :
    );
}

int insert_item(int item) {
    buffer[produce_ptr] = item;
    produce_ptr = (produce_ptr + 1) % BUFFER_SIZE;
    return 0;
}

int remove_item(int *item) {
    *item = buffer[consume_ptr];
    consume_ptr = (consume_ptr + 1) % BUFFER_SIZE;
    return 0;
}

void* producer(void* arg) {
    int item;
    int count = 0;
    while (count < (TOTAL_ELEMENTS / NUM_PRODUCERS)) {
        item = rand(); 

        for (int i = 0; i < 10000; i++);

        sem_wait(&empty);
        lock();

        insert_item(item);
        count++;

        unlock();
        sem_post(&full);
    }
    return NULL;
}

void* consumer(void* arg) {
    int item;
    int count = 0;
    while (count < (TOTAL_ELEMENTS / NUM_CONSUMERS)) {
        for (int i = 0; i < 10000; i++);

        sem_wait(&full);
        lock();

        remove_item(&item);
        count++;

        unlock();
        sem_post(&empty);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        return 1;
    }

    int num_producers = atoi(argv[1]);
    int num_consumers = atoi(argv[2]);

    // int num_producers = NUM_PRODUCERS;
    // int num_consumers = NUM_CONSUMERS;

    pthread_t producers[num_producers];
    pthread_t consumers[num_consumers];

    mutex = 0;
    sem_init(&empty, 0, N);
    sem_init(&full, 0, 0);


    int err;

    for (int i = 0; i < num_producers; i++) {
        err = pthread_create(&producers[i], NULL, producer, NULL);
        if(err!=0){
            return 1;
        }
    }

    for (int i = 0; i < num_consumers; i++) {
        err = pthread_create(&consumers[i], NULL, consumer, NULL);
        if(err!=0){
            return 1;
        }
    }

    for (int i = 0; i < num_producers; i++) {
        err = pthread_join(producers[i], NULL);
        if(err!=0){
            return 1;
        }
    }

    for (int i = 0; i < num_consumers; i++) {
        err = pthread_join(consumers[i], NULL);
        if(err!=0){
            return 1;
        }
    }

    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}
