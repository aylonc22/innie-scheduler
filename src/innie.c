#include "innie.h"
#include <stdio.h>

void init_innie(Innie *i, int id) {
    i->pc = 0;
    i->work_value = 0;
    i->id = id;
}

void* worker_routine(void* arg) {
    Innie *i = (Innie*)arg;
    i->work_value += 1;
    printf("Innie %d worked, value=%d\n", i->id, i->work_value);
    return NULL;
}
