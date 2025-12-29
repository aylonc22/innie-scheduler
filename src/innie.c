#include <stdio.h>
#include <stdlib.h>
#include "innie.h"
#include "registery.h"
#include "instruction.h"
#include "execute.h"

void *innie_worker(void *arg) {
    Innie *innie = (Innie *)arg;

    printf("Innie %s starting\n", innie->name);

    innie->pc = 0;
    while (innie->pc < innie->instructions_count) {
        Instruction *instr = &innie->instructions[innie->pc];
        execute_instruction(innie, instr);
        innie->pc++;
    }

    printf("Innie %s finished with work_value = %d\n",
            innie->name, innie->work_value);

    return NULL;
}

