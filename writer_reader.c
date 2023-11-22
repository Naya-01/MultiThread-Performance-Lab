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

pthread_mutex_t mutex; // mutex lecteur
pthread_mutex_t ecrivain;
pthread_mutex_t bReader; // bloque les reader

sem_t db; // accès à la db
sem_t rsem;

int readcount = 0; // nombre de readers
int writecount = 0;

void read_database()
{
    printf("Lecteur %ld lit la base de données\n", pthread_self());
}

void write_database()
{
    printf("Écrivain %ld écrit dans la base de données\n", pthread_self());
}

void *writer(void *arg)
{
    for (int i = 0; i < WRITING; i++)
    {
        pthread_mutex_lock(&ecrivain);

        writecount++;
        if (writecount == 1)
        {
            sem_wait(&rsem);
        }

        pthread_mutex_unlock(&ecrivain);

        sem_wait(&db);
        write_database();
        sem_post(&db);

        pthread_mutex_lock(&ecrivain);
        writecount--;

        if (writecount == 0)
        {
            sem_post(&rsem);
        }
        pthread_mutex_unlock(&ecrivain);
    }
    return NULL;
}

void *reader(void *arg)
{
    for (int i = 0; i < READING; i++)
    {
        pthread_mutex_lock(&bReader);

        sem_wait(&rsem);

        pthread_mutex_lock(&mutex);
        readcount++;
        if (readcount == 1)
        {
            sem_wait(&db);
        }
        pthread_mutex_unlock(&mutex);
        sem_post(&rsem);

        pthread_mutex_unlock(&bReader);
        read_database();
        pthread_mutex_lock(&mutex);

        readcount--;
        if (readcount == 0)
        {
            sem_post(&db);
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(int argc, char const *argv[])
{
    // if (argc < 3) {
    //     fprintf(stderr, "Usage: %s nombre_ecrivains nombre_lecteurs\n", argv[0]);
    //     return 1;
    // }

    // int nb_ecrivains = atoi(argv[1]);
    // int nb_lecteurs = atoi(argv[2]);

    int nb_ecrivains = NB_WRITER;
    int nb_lecteurs = NB_READER;

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&ecrivain, NULL);
    pthread_mutex_init(&bReader, NULL);
    sem_init(&db, 0, 1);
    sem_init(&rsem, 0, 1);

    pthread_t threads_ecrivains[nb_ecrivains];
    pthread_t threads_lecteurs[nb_lecteurs];

    int err;

    for (int i = 0; i < nb_ecrivains; i++)
    {
        err = pthread_create(&threads_ecrivains[i], NULL, writer, NULL);
        if(err!=0){
            perror("pthread_create producers");
            return 1;
        }
    }
    for (int i = 0; i < nb_lecteurs; i++)
    {
        err = pthread_create(&threads_lecteurs[i], NULL, reader, NULL);
        if(err!=0){
            perror("pthread_create producers");
            return 1;
        }
    }

    for (int i = 0; i < nb_ecrivains; i++)
    {
        err = pthread_join(threads_ecrivains[i], NULL);
        if(err!=0){
            perror("pthread_create producers");
            return 1;
        }
    }
    for (int i = 0; i < nb_lecteurs; i++)
    {
        err = pthread_join(threads_lecteurs[i], NULL);
        if(err!=0){
            perror("pthread_create producers");
            return 1;
        }
    }

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&ecrivain);
    pthread_mutex_destroy(&bReader);
    sem_destroy(&db);
    sem_destroy(&rsem);

    return 0;
}
