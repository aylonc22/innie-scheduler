#ifndef INNIE_H
#define INNIE_H

#include <pthread.h>
#include "instruction.h"

#define INNIE_NAME_MAX 32

typedef struct ShiftStack ShiftStack;

typedef struct Innie{
    pthread_t thread;
    int id;
    char name[32];
    int pc;
    int work_value;
    int waffled; // 0 false 1 true

    char *schedule_src;

    Instruction *instructions;
    int instructions_count;

    ShiftStack *shifts;
} Innie;

void *innie_worker(void *arg);

#endif
