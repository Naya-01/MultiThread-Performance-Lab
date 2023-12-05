#include <stdio.h>
#include <stdlib.h>

#include "verrou.h"

void lock(volatile int *var) {
    int prev_value;
    do {
        #ifdef TATAS
        while (*var==1){}; // Test-and-test-and-set (spin)
        #endif
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
