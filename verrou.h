#ifndef _VERROU_H_
#define _VERROU_H_

void lock(volatile int *var);
void unlock(volatile int *var);

#endif