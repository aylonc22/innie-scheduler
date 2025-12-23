#ifndef INSTRUCTION_H
#define INSTRUCTION_H

typedef enum {
    INST_NOOP = 0,
    INST_ADD,
    INST_MULTIPLY,
    INST_WAFFLE,
    INST_SHIFT,
    INST_ANY_OF,
    INST_ALL_OF
} InstructionType;

typedef struct {
    InstructionType type;
    int arg_count;
    int args[4];
} Instruction;

#endif