#include <stdlib.h>
#include "../include/shift_stack.h"

ShiftStack *shift_stack_create(void) {
    ShiftStack *s = malloc(sizeof(ShiftStack));
    s->top = -1;
    return s;
}

void shift_stack_destroy(ShiftStack *stack) {
    free(stack);
}

void shift_push(ShiftStack *stack, int start_pc, int count) {
    stack->top++;
    stack->frames[stack->top].start_pc = start_pc;
    stack->frames[stack->top].remaining = count;
}

int shift_pop(ShiftStack *stack) {
    if (stack->top < 0) return 0;
    stack->top--;
    return 1;
}

ShiftFrame *shift_peek(ShiftStack *stack) {
    if (stack->top < 0) return NULL;
    return &stack->frames[stack->top];
}
