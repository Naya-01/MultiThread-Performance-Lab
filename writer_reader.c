#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

pthread_mutex_t mutex; // mutex lecteur
pthread_mutex_t ecrivain;
pthread_mutex_t bReader; // bloque les reader

sem_t db; // accès à la db
sem_t rsem;

int readcount = 0; // nombre de readers
int writecount = 0;

void writer(void)
{
    while (true)
    {
        prepare_data();
        pthread_mutex_lock(&ecrivain);

        writecount++;
        if (writecount == 1)
        {
            sem_wait(&rsem);
        }

        pthread_mutex_unlock(&ecrivain);

        sem_wait(&db);
        // section critique, un seul writer à la fois
        write_database();
        sem_post(&db);

        writecount--;

        if (writecount == 0)
        {
            sem_post(&rsem);
        }
        pthread_mutex_unlock(&ecrivain);
    }
}

void reader(void)
{
    while (true)
    {
        pthread_mutex_lock(&bReader);

        sem_wait(&rsem);

        pthread_mutex_lock(&mutex);
        // section critique
        readcount++;
        if (readcount == 1)
        { // arrivée du premier reader
            sem_wait(&db);
        }
        pthread_mutex_unlock(&mutex);
        sem_post(&rsem);

        pthread_mutex_unlock(&bReader);
        read_database();
        pthread_mutex_lock(&mutex);
        // section critique
        readcount--;
        if (readcount == 0)
        { // départ du dernier reader
            sem_post(&db);
        }
        pthread_mutex_unlock(&mutex);
        process_data();
    }
}

int main(int argc, char const *argv[])
{

    sem_init(&db, 0, 1);
    sem_init(&ecrivain, 0, 1);
    /* code */
    return 0;
}
