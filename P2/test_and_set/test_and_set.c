#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>


#define LOOPS 6400

volatile int shared_lock = 0;
int num_thread;
int count = 0;
void lock() {
    int prev_value;
    do {
        __asm__ __volatile__(
            "xchg %0, %1\n\t"
            : "=r"(prev_value), "+m"(shared_lock)
            : "0"(1)
            :
        );
    } while (prev_value == 1);
}

void unlock() {
    __asm__ __volatile__(
        "movl $0, %0\n\t"
        : "+m"(shared_lock)
        :
        :
    );
}

void section_crique()
{
    for (int i = 0; i < 10000; i++); // Emulation de la section critique
}

void *thread_code(void *arg){
    int num_thread = *((int *) arg); 
    for (int i = 0; i < LOOPS/num_thread; i++)
    {
        lock();
        section_crique();
        unlock();
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

    printf("%d\n", count);
}