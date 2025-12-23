#ifndef SHIFT_STACK_H
#define SHIFT_STACK_H

typedef struct ShiftFrame{
    int start_pc;
    int remaining;
} ShiftFrame;

typedef struct ShiftStack {
    ShiftFrame frames[64];
    int top;
} ShiftStack;

ShiftStack *shift_stack_create(void);
void shift_stack_destroy(ShiftStack *stack);

void shift_push(ShiftStack *stack, int start_pc, int count);
int shift_pop(ShiftStack *stack);
ShiftFrame *shift_peek(ShiftStack *stack);

#endif
