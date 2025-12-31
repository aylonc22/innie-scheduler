#ifndef INNIE_H
#define INNIE_H

#include <pthread.h>
#include "instruction.h"

#define INNIE_NAME_MAX 32

typedef struct ShiftStack ShiftStack;

typedef enum {
    INNIE_RUNNING,
    INNIE_WAITING,
    INNIE_FINISHED,
    INNIE_DEADLOCKED
} InnieState;

typedef struct Innie{
    pthread_t thread;
    int id;
    char name[32];
    int pc;
    int work_value;
    InnieState state;

    char *schedule_src;

    Instruction *instructions;
    int instructions_count;

    ShiftStack *shifts;

    struct Innie **waiting_for;
    int waiting_count;
} Innie;

void *innie_worker(void *arg);

#endif
