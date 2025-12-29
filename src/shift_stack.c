#include <stdlib.h>
#include <stdio.h>
#include "shift_stack.h"

ShiftStack *shift_stack_create(void) {
    ShiftStack *s = malloc(sizeof(ShiftStack));
    if (!s) {
        perror("malloc");
        exit(1);
    }
    s->top = -1;
    return s;
}

void shift_stack_destroy(ShiftStack *stack) {
    free(stack);
}

int shift_stack_empty(ShiftStack *stack) {
    return stack->top < 0;
}

void shift_stack_push(ShiftStack *stack, int start_pc, int count) {
    if (stack->top >= MAX_SHIFTS - 1) {
        fprintf(stderr, "SHIFT stack overflow\n");
        exit(1);
    }

    stack->top++;
    stack->frames[stack->top].start_pc = start_pc;
    stack->frames[stack->top].remaining = count;
}

void shift_stack_pop(ShiftStack *stack) {
    if (stack->top < 0) {
        fprintf(stderr, "SHIFT stack underflow\n");
        return;
    }
    stack->top--;
}

ShiftFrame *shift_stack_peek(ShiftStack *stack) {
    if (stack->top < 0) return NULL;
    return &stack->frames[stack->top];
}
