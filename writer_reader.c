#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define WRITING 640
#define READING 2560
#define NB_READER 1
#define NB_WRITER 10

#ifdef MUTEX
pthread_mutex_t mutex; // mutex lecteur
pthread_mutex_t ecrivain;
pthread_mutex_t bReader; // bloque les reader
#else
volatile int mutex = 0;
volatile int ecrivain = 0;
volatile int bReader = 0;
#endif

sem_t db; // accès à la db
sem_t rsem;

int readcount = 0; // nombre de readers
int writecount = 0;

#ifndef MUTEX
void lock(volatile int *var) {
    int prev_value;
    do {
        __asm__ __volatile__(
            "xchg %0, %1\n\t"
            : "=r"(prev_value), "+m"(*var)
            : "0"(1)
            :
        );
    } while (prev_value == 1);
}

void unlock(volatile int *var) {
    __asm__ __volatile__(
        "movl $0, %0\n\t"
        : "+m"(*var)
        :
        :
    );
}
#endif

void read_database()
{
}

void write_database()
{
}

void *writer(void *arg)
{
    for (int i = 0; i < WRITING; i++)
    {
        #ifdef MUTEX
        pthread_mutex_lock(&ecrivain);
        #else
        lock(&ecrivain);
        #endif

        writecount++;
        if (writecount == 1)
        {
            sem_wait(&rsem);
        }

        #ifdef MUTEX
        pthread_mutex_unlock(&ecrivain);
        #else
        unlock(&ecrivain);
        #endif

        sem_wait(&db);
        write_database();
        sem_post(&db);

        #ifdef MUTEX
        pthread_mutex_lock(&ecrivain);
        #else
        lock(&ecrivain);
        #endif
        writecount--;

        if (writecount == 0)
        {
            sem_post(&rsem);
        }

        #ifdef MUTEX
        pthread_mutex_unlock(&ecrivain);
        #else
        unlock(&ecrivain);
        #endif
    }
    return NULL;
}

void *reader(void *arg)
{
    for (int i = 0; i < READING; i++)
    {
        #ifdef MUTEX
        pthread_mutex_lock(&bReader);
        #else
        lock(&bReader);
        #endif

        sem_wait(&rsem);

        #ifdef MUTEX
        pthread_mutex_lock(&mutex);
        #else
        lock(&mutex);
        #endif

        readcount++;
        if (readcount == 1)
        {
            sem_wait(&db);
        }

        #ifdef MUTEX
        pthread_mutex_unlock(&mutex);
        #else
        unlock(&mutex);
        #endif
        
        sem_post(&rsem);

        #ifdef MUTEX
        pthread_mutex_unlock(&bReader);
        #else
        unlock(&bReader);
        #endif
        
        read_database();

        #ifdef MUTEX
        pthread_mutex_lock(&mutex);
        #else
        lock(&mutex);
        #endif

        readcount--;
        if (readcount == 0)
        {
            sem_post(&db);
        }

        #ifdef MUTEX
        pthread_mutex_unlock(&mutex);
        #else
        unlock(&mutex);
        #endif
    }
    return NULL;
}

int main(int argc, char const *argv[])
{
    if (argc < 3) {
        return 1;
    }

    int nb_ecrivains = atoi(argv[1]);
    int nb_lecteurs = atoi(argv[2]);

    // int nb_ecrivains = NB_WRITER;
    // int nb_lecteurs = NB_READER;

    #ifdef MUTEX
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&ecrivain, NULL);
    pthread_mutex_init(&bReader, NULL);
    #else
    mutex = 0;
    #endif

    sem_init(&db, 0, 1);
    sem_init(&rsem, 0, 1);

    pthread_t threads_ecrivains[nb_ecrivains];
    pthread_t threads_lecteurs[nb_lecteurs];

    int err;

    for (int i = 0; i < nb_ecrivains; i++)
    {
        err = pthread_create(&threads_ecrivains[i], NULL, writer, NULL);
        if(err!=0){
            return 1;
        }
    }
    for (int i = 0; i < nb_lecteurs; i++)
    {
        err = pthread_create(&threads_lecteurs[i], NULL, reader, NULL);
        if(err!=0){
            return 1;
        }
    }

    for (int i = 0; i < nb_ecrivains; i++)
    {
        err = pthread_join(threads_ecrivains[i], NULL);
        if(err!=0){
            return 1;
        }
    }
    for (int i = 0; i < nb_lecteurs; i++)
    {
        err = pthread_join(threads_lecteurs[i], NULL);
        if(err!=0){
            return 1;
        }
    }

    #ifdef MUTEX
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&ecrivain);
    pthread_mutex_destroy(&bReader);
    #endif
    
    sem_destroy(&db);
    sem_destroy(&rsem);

    return 0;
}
