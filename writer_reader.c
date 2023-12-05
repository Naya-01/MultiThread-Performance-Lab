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

#ifndef POSIX
#include "verrou.h"
#include "mysemaphore.h"
#endif

#ifdef POSIX
pthread_mutex_t mutex; // mutex lecteur
pthread_mutex_t ecrivain;
pthread_mutex_t bReader; // bloque les reader

sem_t db; // accès à la db
sem_t rsem;
#else
volatile int mutex = 0;
volatile int ecrivain = 0;
volatile int bReader = 0;

my_semaphore* db;
my_semaphore* rsem;
#endif

int readcount = 0; // nombre de readers
int writecount = 0;

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
        #ifdef POSIX
        pthread_mutex_lock(&ecrivain);
        #else
        lock(&ecrivain);
        #endif

        writecount++;
        if (writecount == 1)
        {
            #ifdef POSIX
            sem_wait(&rsem);
            #else
            wait(rsem);
            #endif
        }

        #ifdef POSIX
        pthread_mutex_unlock(&ecrivain);

        sem_wait(&db);
        write_database();
        sem_post(&db);

        pthread_mutex_lock(&ecrivain);
        #else
        unlock(&ecrivain);

        wait(db);
        write_database();
        post(db);

        lock(&ecrivain);
        #endif

        writecount--;

        if (writecount == 0)
        {
            #ifdef POSIX
            sem_post(&rsem);
            #else
            post(rsem);
            #endif
        }

        #ifdef POSIX
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
        #ifdef POSIX
        pthread_mutex_lock(&bReader);

        sem_wait(&rsem);
        #else
        lock(&bReader);

        wait(rsem);
        #endif

        #ifdef POSIX
        pthread_mutex_lock(&mutex);
        #else
        lock(&mutex);
        #endif

        readcount++;
        if (readcount == 1)
        {
            #ifdef POSIX
            sem_wait(&db);
            #else
            wait(db);
            #endif
        }

        #ifdef POSIX
        pthread_mutex_unlock(&mutex);

        sem_post(&rsem);

        pthread_mutex_unlock(&bReader);
        #else
        unlock(&mutex);

        post(rsem);

        unlock(&bReader);
        #endif
        
        read_database();

        #ifdef POSIX
        pthread_mutex_lock(&mutex);
        #else
        lock(&mutex);
        #endif

        readcount--;
        if (readcount == 0)
        {
            #ifdef POSIX
            sem_post(&db);
            #else
            post(db);
            #endif
        }

        #ifdef POSIX
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

    #ifdef POSIX
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&ecrivain, NULL);
    pthread_mutex_init(&bReader, NULL);

    sem_init(&db, 0, 1);
    sem_init(&rsem, 0, 1);
    #else
    mutex = 0;

    db = (my_semaphore*) malloc(sizeof (my_semaphore));
    rsem = (my_semaphore*) malloc(sizeof (my_semaphore));
    init(db, 1);
    init(rsem, 1);
    #endif

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

    #ifdef POSIX
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&ecrivain);
    pthread_mutex_destroy(&bReader);

    sem_destroy(&db);
    sem_destroy(&rsem);
    #else
    destroy(db);
    destroy(rsem);
    #endif

    return 0;
}
