#ifndef INNIE_H
#define INNIE_H

#include <pthread.h>
#include "instruction.h"

#define INNIE_NAME_MAX 32

typedef struct ShiftStack ShiftStack;

typedef struct Innie{
    char id;
    pthread_t thread;

    int pc;             
    int work_value;     

    char name[INNIE_NAME_MAX];

    ShiftStack *shifts;
    Instruction *instructions;
    int instructions_count;
} Innie;

void *innie_worker(void *arg);

#endif
