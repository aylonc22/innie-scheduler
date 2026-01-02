#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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
            for (int i = 0; i < arg->list.count; i++)
                sum += arg->list.innies[i]->work_value;
            return sum;
        }
        default:
            fprintf(stderr, "[%s] Invalid argument type\n", self->name);
            return 0;
    }
}

/* ---------------- CONDITION EVALUATION ---------------- */
static bool eval_condition(int lhs, ConditionType type, Arg *rhs, bool any_of) {
    switch (rhs->type) {
        case ARG_INT: {
            int r = rhs->int_value;
            switch (type) {
                case COND_GT: return lhs > r;
                case COND_LT: return lhs < r;
                case COND_EQ: return lhs == r;
            }
            break;
        }
        case ARG_INNIE: {
            int r = rhs->innie->work_value;
            switch (type) {
                case COND_GT: return lhs > r;
                case COND_LT: return lhs < r;
                case COND_EQ: return lhs == r;
            }
            break;
        }
        case ARG_LIST: {
            int satisfied = 0;
            for (int i = 0; i < rhs->list.count; i++) {
                int r = rhs->list.innies[i]->work_value;
                bool ok = false;
                switch (type) {
                    case COND_GT: ok = lhs > r; break;
                    case COND_LT: ok = lhs < r; break;
                    case COND_EQ: ok = lhs == r; break;
                }
                if (ok) satisfied++;
            }
            if (any_of) return satisfied > 0;
            else return satisfied == rhs->list.count;
        }
        default: return false;
    }
    return false;
}

/* ---------------- EXECUTION ---------------- */
ExecResult execute_instruction(Innie *innie, Instruction *instr) {
    innie->waiting_count = 0; // reset each instruction

    /* Helper: check dependencies */
    for (int i = 0; i < instr->arg_count; i++) {
        Arg *arg = &instr->args[i];
        if (arg->type == ARG_INNIE && arg->innie->state != INNIE_FINISHED)
            innie->waiting_for[innie->waiting_count++] = arg->innie;
        else if (arg->type == ARG_LIST) {
            for (int j = 0; j < arg->list.count; j++) {
                if (arg->list.innies[j]->state != INNIE_FINISHED)
                    innie->waiting_for[innie->waiting_count++] = arg->list.innies[j];
            }
        }
    }

    if (instr->cond) {
        Arg *lhs_arg = &instr->cond->lhs;
        Arg *rhs_arg = &instr->cond->rhs;

        // LHS dependencies
        if (lhs_arg->type == ARG_INNIE && lhs_arg->innie->state != INNIE_FINISHED)
            innie->waiting_for[innie->waiting_count++] = lhs_arg->innie;
        if (lhs_arg->type == ARG_LIST) {
            for (int i = 0; i < lhs_arg->list.count; i++)
                if (lhs_arg->list.innies[i]->state != INNIE_FINISHED)
                    innie->waiting_for[innie->waiting_count++] = lhs_arg->list.innies[i];
        }

        // RHS dependencies
        if (rhs_arg->type == ARG_INNIE && rhs_arg->innie->state != INNIE_FINISHED)
            innie->waiting_for[innie->waiting_count++] = rhs_arg->innie;
        if (rhs_arg->type == ARG_LIST) {
            for (int i = 0; i < rhs_arg->list.count; i++)
                if (rhs_arg->list.innies[i]->state != INNIE_FINISHED)
                    innie->waiting_for[innie->waiting_count++] = rhs_arg->list.innies[i];
        }
    }

    if (innie->waiting_count > 0) {
        innie->state = INNIE_WAITING;
        return EXEC_BLOCKED;
    }

    switch (instr->type) {

    case INST_LOAD: {
        int value = resolve_arg_value(innie, &instr->args[0]);
        printf("[%s] LOAD %d\n", innie->name, value);
        innie->work_value = value;
        innie->state = INNIE_RUNNING;
        return EXEC_OK;
    }

    case INST_ADD:
    case INST_MODULO:
    case INST_MULTIPLY: {
        int value = resolve_arg_value(innie, &instr->args[0]);
        switch (instr->type) {
            case INST_ADD:
                printf("[%s] ADD %d\n", innie->name, value);
                innie->work_value += value;
                break;

            case INST_MULTIPLY:
                printf("[%s] MULTIPLY %d\n", innie->name, value);
                innie->work_value *= value;
                break;

            case INST_MODULO:
                if (value == 0) {
                    fprintf(stderr, "[%s] MODULO by zero, skipping\n", innie->name);
                } else {
                    printf("[%s] MODULO %d\n", innie->name, value);
                    innie->work_value %= value;
                }
                break;

            default:
                fprintf(stderr, "[%s] Unknown arithmetic instruction %d\n", innie->name, instr->type);
                break;
        }
        innie->state = INNIE_RUNNING;
        return EXEC_OK;
    }

    case INST_WAFFLE:
        printf("[%s] WAFFLE → %d\n", innie->name, innie->work_value);
        registry_set(&registry, innie->id, innie->work_value);
        innie->state = INNIE_FINISHED;
        return EXEC_TERMINATE;

    case INST_SHIFT: {
        int times = instr->args[0].int_value;
        printf("[%s] SHIFT %d TIMES\n", innie->name, times);
        if (times > 0)
            shift_stack_push(innie->shifts, innie->pc + 1, times);
        return EXEC_OK;
    }

    case INST_END_SHIFT: {
        ShiftFrame *frame = shift_stack_peek(innie->shifts);
        if (!frame) {
            fprintf(stderr, "[%s] END_SHIFT without SHIFT\n", innie->name);
            return EXEC_OK;
        }
        frame->remaining--;
        if (frame->remaining > 0)
            innie->pc = frame->start_pc - 1;
        else
            shift_stack_pop(innie->shifts);
        return EXEC_OK;
    }

    case INST_WELLNESS_CHECK: {
        int lhs = resolve_arg_value(innie, &instr->cond->lhs);
        bool any_of = instr->cond->rhs.type == ARG_LIST && instr->cond->rhs.list.count > 1;
        bool ok = eval_condition(lhs, instr->cond->type, &instr->cond->rhs, any_of);
        printf("[%s] WELLNESS_CHECK %s\n", innie->name, ok ? "PASSED" : "FAILED");
        return EXEC_OK;
    }

    case INST_CONDITIONAL_ADD: {
        int lhs = resolve_arg_value(innie, &instr->cond->lhs);
        bool any_of = instr->cond->rhs.type == ARG_LIST && instr->cond->rhs.list.count > 1;
        bool condition_met = eval_condition(lhs, instr->cond->type, &instr->cond->rhs, any_of);

        if (condition_met) {
            int add = resolve_arg_value(innie, &instr->args[0]);
            printf("[%s] CONDITIONAL_ADD → %d\n", innie->name, add);
            innie->work_value += add;
        } else {
            printf("[%s] CONDITIONAL_ADD skipped\n", innie->name);
        }
        return EXEC_OK;
    }

    default:
        fprintf(stderr, "[%s] Unknown instruction %d\n", innie->name, instr->type);
        return EXEC_OK;
    }
}
