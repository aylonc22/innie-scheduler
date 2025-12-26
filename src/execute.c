#include "innie.h"
#include "registery.h"
#include "instruction.h"
#include <stdio.h>

void execute_instruction(Innie *innie, Instruction *instr) {
    switch(instr->type) {
        case INST_ADD:
            // For now, assume args[0] is raw integer
            innie->work_value += instr->args[0];
            break;
        case INST_MULTIPLY:
            innie->work_value *= instr->args[0];
            break;
        case INST_WAFFLE:
            registry_set(&registry, innie->id, innie->work_value);
            break;
        case INST_SHIFT:
            // Shift stack logic later
            break;
        case INST_ANY_OF:
        case INST_ALL_OF:
            // Condition logic later
            break;
        default:
            break;
    }
}
