#include <stdio.h>
#include <stdlib.h>
#include "innie.h"
#include "registery.h"
#include "instruction.h"
#include "execute.h"

void *innie_worker(void *arg) {
    Innie *innie = (Innie *)arg;

    printf("Innie %s starting\n", innie->name);

    // Loop through instructions
    for (int pc = 0; pc < innie->instructions_count; pc++) {
        Instruction *instr = &innie->instructions[pc];
        execute_instruction(innie, instr);
    }

    printf("Innie %s finished with work_value = %d\n",
            innie->name, innie->work_value);

    return NULL;
}
