#include <stdio.h>
#include <stdlib.h>
#include "../include/innie.h"
#include "../include/registery.h"
#include "../include/instruction.h"
#include "../include/execute.h"

Registry registry;

void *innie_worker(void *arg) {
    Innie *innie = (Innie *)arg;

    printf("Innie %d starting\n", innie->id);

    // Loop through instructions
    for (int pc = 0; pc < innie->instruction_count; pc++) {
        Instruction *instr = &innie->instruction[pc];
        execute_instruction(innie, instr);
    }

    printf("Innie %d finished with work_value = %d\n",
            innie->id, innie->work_value);

    return NULL;
}
