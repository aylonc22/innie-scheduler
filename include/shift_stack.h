#ifndef SHIFT_STACK_H
#define SHIFT_STACK_H

#define MAX_SHIFTS 32

typedef struct ShiftFrame{
    int start_pc;
    int remaining;
} ShiftFrame;

typedef struct ShiftStack{
    ShiftFrame frames[MAX_SHIFTS];
    int top;
} ShiftStack;

ShiftStack *shift_stack_create(void);
void shift_stack_destroy(ShiftStack *stack);

void shift_stack_push(ShiftStack *stack, int start_pc, int count);
void shift_stack_pop(ShiftStack *stack);
ShiftFrame *shift_stack_peek(ShiftStack *stack);
int shift_stack_empty(ShiftStack *stack);

#endif
