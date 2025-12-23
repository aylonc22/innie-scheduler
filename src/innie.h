#ifndef INNIE_H
#define INNIE_H

#include <pthread.h>

typedef struct Innie{
    pthread_t thread;
    int pc;
    int work_value;
    int id;
} Innie;

void init_innie(Innie *i, int id);
void* worker_routine(void* arg);

#endif
