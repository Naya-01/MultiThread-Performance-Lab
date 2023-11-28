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

volatile int mutex = 0;
volatile int ecrivain = 0;
volatile int bReader = 0;

sem_t db; // accès à la db
sem_t rsem;

int readcount = 0; // nombre de readers
int writecount = 0;

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
        lock(&ecrivain);

        writecount++;
        if (writecount == 1)
        {
            sem_wait(&rsem);
        }

        unlock(&ecrivain);

        sem_wait(&db);
        write_database();
        sem_post(&db);

        lock(&ecrivain);
        writecount--;

        if (writecount == 0)
        {
            sem_post(&rsem);
        }
        unlock(&ecrivain);
    }
    return NULL;
}

void *reader(void *arg)
{
    for (int i = 0; i < READING; i++)
    {
        lock(&bReader);

        sem_wait(&rsem);

        lock(&mutex);
        readcount++;
        if (readcount == 1)
        {
            sem_wait(&db);
        }
        unlock(&mutex);
        sem_post(&rsem);

        unlock(&bReader);
        read_database();
        lock(&mutex);

        readcount--;
        if (readcount == 0)
        {
            sem_post(&db);
        }
        unlock(&mutex);
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
    mutex = 0;
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

    sem_destroy(&db);
    sem_destroy(&rsem);

    return 0;
}
