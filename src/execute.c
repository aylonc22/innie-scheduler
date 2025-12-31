#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include "innie.h"
#include "registery.h"
#include "instruction.h"
#include "shift_stack.h"
#include "execute.h"

/* ---------------- ARG RESOLUTION ---------------- */

static int resolve_arg_value(Innie *self, Arg *arg) {
    (void) self;
    switch (arg->type) {

        case ARG_INT:
            return arg->int_value;

        case ARG_INNIE:
            return arg->innie->work_value;

        case ARG_LIST: {
            int sum = 0;
            for (int i = 0; i < arg->list.count; i++) {
                sum += arg->list.innies[i]->work_value;
            }
            return sum;
        }

        default:
            fprintf(stderr, "Invalid argument type\n");
            return 0;
    }
}

/* ---------------- EXECUTION ---------------- */

ExecResult execute_instruction(Innie *innie, Instruction *instr) {

    switch (instr->type) {

        case INST_LOAD: {
            int value = resolve_arg_value(innie, &instr->args[0]);
            printf("[%s] LOAD %d\n", innie->name, value);
            innie->work_value = value;
            innie->state = INNIE_RUNNING;
            innie->waiting_count = 0; // no dependencies
            return EXEC_OK;
        }

        case INST_ADD:
        case INST_MULTIPLY: {
            // Reset waiting list
            innie->waiting_count = 0;

            if (instr->args[0].type == ARG_LIST) {
                for (int i = 0; i < instr->args[0].list.count; i++) {
                    Innie *dep = instr->args[0].list.innies[i];
                    if (dep->state != INNIE_FINISHED) {
                        innie->waiting_for[innie->waiting_count++] = dep;
                    }
                }
                if (innie->waiting_count > 0) {
                    innie->state = INNIE_WAITING;
                    return EXEC_BLOCKED;
                }
            }

            int value = resolve_arg_value(innie, &instr->args[0]);
            if (instr->type == INST_ADD) {
                printf("[%s] ADD %d\n", innie->name, value);
                innie->work_value += value;
            } else {
                printf("[%s] MULTIPLY %d\n", innie->name, value);
                innie->work_value *= value;
            }
            innie->state = INNIE_RUNNING;
            return EXEC_OK;
        }

        case INST_WAFFLE:
            printf("[%s] WAFFLE → %d\n", innie->name, innie->work_value);
            registry_set(&registry, innie->id, innie->work_value);
            innie->state = INNIE_FINISHED;
            innie->waiting_count = 0; // done waiting
            return EXEC_TERMINATE;

        case INST_SHIFT: {
            if (instr->args[0].type != ARG_INT) {
                fprintf(stderr, "[%s] SHIFT expects integer\n", innie->name);
                return EXEC_OK;
            }
            int times = instr->args[0].int_value;
            printf("[%s] SHIFT %d TIMES\n", innie->name, times);
            if (times > 0) {
                shift_stack_push(innie->shifts, innie->pc + 1, times);
            }
            innie->state = INNIE_RUNNING;
            return EXEC_OK;
        }

        case INST_END_SHIFT: {
            ShiftFrame *frame = shift_stack_peek(innie->shifts);
            if (!frame) {
                fprintf(stderr, "[%s] END_SHIFT without SHIFT\n", innie->name);
                return EXEC_OK;
            }
            frame->remaining--;
            if (frame->remaining > 0) {
                innie->pc = frame->start_pc - 1;
            } else {
                shift_stack_pop(innie->shifts);
            }
            innie->state = INNIE_RUNNING;
            return EXEC_OK;
        }

        case INST_ANY_OF:
        case INST_ALL_OF: {
            Arg *list_arg = &instr->args[0];
            innie->waiting_count = 0;

            if (list_arg->type != ARG_LIST) {
                fprintf(stderr, "[%s] %s expects a list argument\n",
                        innie->name,
                        instr->type == INST_ANY_OF ? "ANY_OF" : "ALL_OF");
                return EXEC_OK;
            }

            int ready_count = 0;
            for (int i = 0; i < list_arg->list.count; i++) {
                Innie *dep = list_arg->list.innies[i];
                if (dep->state == INNIE_FINISHED) {
                    ready_count++;
                } else {
                    innie->waiting_for[innie->waiting_count++] = dep;
                }
            }

            int satisfied = (instr->type == INST_ANY_OF && ready_count > 0) ||
                            (instr->type == INST_ALL_OF && ready_count == list_arg->list.count);

            if (satisfied) {
                int value = resolve_arg_value(innie, list_arg);
                printf("[%s] %s → %d\n",
                        innie->name,
                        instr->type == INST_ANY_OF ? "ANY_OF" : "ALL_OF",
                        value);
                innie->work_value += value;
                innie->state = INNIE_RUNNING;
                innie->waiting_count = 0;
                return EXEC_OK;
            } else {
                innie->state = INNIE_WAITING;
                return EXEC_BLOCKED;
            }
        }

        default:
            fprintf(stderr, "[%s] Unknown instruction %d\n", innie->name, instr->type);
            return EXEC_OK;
    }
}
