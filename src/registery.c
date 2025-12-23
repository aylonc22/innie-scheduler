#include "../include/registery.h"

void registry_init(Registry *r) {
    for (int i = 0; i < MAX_INNIES; i++) {
        r->ready[i] = 0;
    }
    pthread_mutex_init(&r->mutex, NULL);
    pthread_cond_init(&r->cond, NULL);
}

void registry_set(Registry *r, int id, int value) {
    pthread_mutex_lock(&r->mutex);

    r->values[id] = value;
    r->ready[id] = 1;

    pthread_cond_broadcast(&r->cond);

    pthread_mutex_unlock(&r->mutex);
}

int registry_get(Registry *r, int id, int *out) {
    pthread_mutex_lock(&r->mutex);

    while (!r->ready[id]) {
        pthread_cond_wait(&r->cond, &r->mutex);
    }

    *out = r->values[id];

    pthread_mutex_unlock(&r->mutex);
    return 1;
}
