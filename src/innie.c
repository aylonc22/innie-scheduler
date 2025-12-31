#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include "innie.h"
#include "registery.h"
#include "instruction.h"
#include "execute.h"

void *innie_worker(void *arg) {
    Innie *innie = (Innie *)arg;

    printf("Innie %s starting\n", innie->name);

    innie->state = INNIE_RUNNING;
    innie->pc = 0;

    while (innie->state != INNIE_FINISHED && innie->state != INNIE_DEADLOCKED) {

        if (innie->pc >= innie->instructions_count) {
            fprintf(stderr, "Innie %s reached end without WAFFLE\n", innie->name);
            innie->state = INNIE_FINISHED;
            break;
        }

        Instruction *instr = &innie->instructions[innie->pc];
        ExecResult res = execute_instruction(innie, instr);

        if (res == EXEC_OK) {
            innie->pc++;
            innie->state = INNIE_RUNNING;
        } else if (res == EXEC_BLOCKED) {
            innie->state = INNIE_WAITING;
            sched_yield();
        } else if (res == EXEC_TERMINATE) {
            innie->state = INNIE_FINISHED;
        }
    }

    if (innie->state == INNIE_DEADLOCKED) {
        printf("Innie %s terminated due to DEADLOCK\n", innie->name);
    } else {
        printf("Innie %s finished with work_value = %d\n",
               innie->name, innie->work_value);
    }

    return NULL;
}
