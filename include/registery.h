#ifndef REGISTRY_H
#define REGISTRY_H

#include <pthread.h>

#define MAX_INNIES 64

typedef struct Registry{
    int values[MAX_INNIES];
    int ready[MAX_INNIES];

    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Registry;

extern Registry registry;

void registry_init(Registry *r);
void registry_set(Registry *r, int id, int value);
int registry_get(Registry *r, int id, int *out);

#endif
