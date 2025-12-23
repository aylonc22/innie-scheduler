#ifndef INNIE_H
#define INNIE_H

#include <pthread.h>
#include "instruction.h"

typedef struct ShiftStack ShiftStack;

typedef struct Innie{
    int id;
    pthread_t thread;

    int pc;             
    int work_value;     

    ShiftStack *shifts;
    Instruction *instruction;
    int instruction_count;
} Innie;

void *innie_worker(void *arg);

#endif
