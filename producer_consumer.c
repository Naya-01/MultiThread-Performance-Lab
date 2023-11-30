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
int num_producers;
int num_consumers;

#ifdef MUTEX
pthread_mutex_t mutex;
#else
volatile int mutex = 0;
#endif
sem_t empty;
sem_t full;


#ifndef MUTEX
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
#endif

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
    int max_items = *(int*)arg;
    while (count < max_items) {
        item = rand(); 

        for (int i = 0; i < 10000; i++);

        sem_wait(&empty);
        #ifdef MUTEX
        pthread_mutex_lock(&mutex);
        #else
        lock();
        #endif

        insert_item(item);
        count++;

        #ifdef MUTEX
        pthread_mutex_unlock(&mutex);
        #else
        unlock();
        #endif

        sem_post(&full);
    }
    return NULL;
}

void* consumer(void* arg) {
    int item;
    int count = 0;
    int max_items = *(int*)arg;
    while (count < max_items) {
        for (int i = 0; i < 10000; i++);

        sem_wait(&full);
        #ifdef MUTEX
        pthread_mutex_lock(&mutex);
        #else
        lock();
        #endif

        remove_item(&item);
        count++;

        #ifdef MUTEX
        pthread_mutex_unlock(&mutex);
        #else
        unlock();
        #endif

        sem_post(&empty);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        return 1;
    }

    num_producers = atoi(argv[1]);
    num_consumers = atoi(argv[2]);
    
    int producer_tasks[num_producers];
    int consumer_tasks[num_consumers];

    for (int i = 0; i < num_producers; i++) {
        producer_tasks[i] = (TOTAL_ELEMENTS / num_producers) + (i < (TOTAL_ELEMENTS % num_producers));
    }

    for (int i = 0; i < num_consumers; i++) {
        consumer_tasks[i] = (TOTAL_ELEMENTS / num_consumers) + (i < (TOTAL_ELEMENTS % num_consumers));
    }

    // int num_producers = NUM_PRODUCERS;
    // int num_consumers = NUM_CONSUMERS;

    pthread_t producers[num_producers];
    pthread_t consumers[num_consumers];

    #ifdef MUTEX
    pthread_mutex_init(&mutex, NULL);
    #else
    mutex = 0;
    #endif
    sem_init(&empty, 0, N);
    sem_init(&full, 0, 0);


    int err;

    for (int i = 0; i < num_producers; i++) {
        err = pthread_create(&producers[i], NULL, producer, &producer_tasks[i]);
        if(err!=0){
            return 1;
        }
    }

    for (int i = 0; i < num_consumers; i++) {
        err = pthread_create(&consumers[i], NULL, consumer, &consumer_tasks[i]);
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

    #ifdef MUTEX
    pthread_mutex_destroy(&mutex);
    #endif

    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}
