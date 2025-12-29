#include <stdio.h>
#include "innie.h"
#include "registery.h"
#include "instruction.h"
#include "shift_stack.h"

// Resolve argument value: respects WAFFLED for lists
static int resolve_arg_value(Innie *self, Arg *arg) {
    switch (arg->type) {

        case ARG_INT:
            return arg->int_value;

        case ARG_INNIE:
            return arg->innie->work_value;

        case ARG_LIST: {
            int sum = 0;
            for (int i = 0; i < arg->list.count; i++) {
                Innie *inn = arg->list.innies[i];
                if (inn->waffled) {   // Only include WAFFLED innies
                    sum += inn->work_value;
                }
            }
            return sum;
        }

        default:
            fprintf(stderr, "Invalid argument type\n");
            return 0;
    }
}

void execute_instruction(Innie *innie, Instruction *instr) {
    switch (instr->type) {

        case INST_LOAD: {
            int value = resolve_arg_value(innie, &instr->args[0]);
            printf("%s: LOAD %d\n", innie->name, value);
            innie->work_value = value;
            break;
        }

        case INST_ADD: {
            int value = resolve_arg_value(innie, &instr->args[0]);
            printf("%s: ADD %d\n", innie->name, value);
            innie->work_value += value;
            break;
        }

        case INST_MULTIPLY: {
            int value = resolve_arg_value(innie, &instr->args[0]);
            printf("%s: MULTIPLY %d\n", innie->name, value);
            innie->work_value *= value;
            break;
        }

        case INST_WAFFLE:
            printf("%s: WAFFLE (work_value=%d)\n", innie->name, innie->work_value);
            innie->waffled = 1;  // mark as done
            registry_set(&registry, innie->id, innie->work_value);
            break;

        case INST_SHIFT: {
            if (instr->args[0].type != ARG_INT) {
                fprintf(stderr, "SHIFT expects integer\n");
                break;
            }
            int times = instr->args[0].int_value;
            printf("%s: SHIFT %d TIMES\n", innie->name, times);
            if (times > 0) {
                shift_stack_push(innie->shifts, innie->pc + 1, times);
            }
            break;
        }

        case INST_END_SHIFT: {
            ShiftFrame *frame = shift_stack_peek(innie->shifts);
            if (!frame) {
                fprintf(stderr, "%s: END_SHIFT without SHIFT\n", innie->name);
                break;
            }
            printf("%s: END_SHIFT\n", innie->name);
            frame->remaining--;
            if (frame->remaining > 0) {
                innie->pc = frame->start_pc - 1;
            } else {
                shift_stack_pop(innie->shifts);
            }
            break;
        }

        case INST_ANY_OF:
        case INST_ALL_OF:
            // To be implemented later
            break;

        default:
            printf("%s: Unknown instruction %d\n", innie->name, instr->type);
            break;
    }
}
