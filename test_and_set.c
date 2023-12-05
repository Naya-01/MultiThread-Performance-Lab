#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#include "verrou.h"

#define LOOPS 6400

volatile int shared_lock = 0;
int num_thread;

void section_crique()
{
    for (int i = 0; i < 10000; i++); // Emulation de la section critique
}

void *thread_code(void *arg){
    int num_thread = *((int *) arg); 
    for (int i = 0; i < LOOPS/num_thread; i++)
    {
        lock(&shared_lock);
        section_crique();
        unlock(&shared_lock);
    }
}

int main(int argc, char* argv[]) {

    // Vérifier si le nombre de tread a bien été donnée en paramètre
    if (argc != 2) {
        return 1;
    }

    // Recuperer l'argument donné en ligne de commande : Nombre de threads
    num_thread = atoi(argv[1]);

    pthread_t threads_bloc[num_thread];

    int err;

    // Creation des threads [Ready]
    for (int i = 0; i < num_thread; i++)
    {
        err = pthread_create(&threads_bloc[i], NULL, thread_code,(void *) &num_thread);
        if(err!=0){
            return 1;
        }
    }

    // Attentes que tout les threads finissent leur travai
    for (int i = 0; i < num_thread; i++)
    {
        err = pthread_join(threads_bloc[i], NULL);
        if(err!=0){
            return 1;
        }
    }
}