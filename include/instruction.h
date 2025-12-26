#ifndef INSTRUCTION_H
#define INSTRUCTION_H

typedef enum {
    INST_LOAD,
    INST_ADD,
    INST_MULTIPLY,
    INST_WAFFLE,
    INST_SHIFT,
    INST_END_SHIFT,
    INST_ANY_OF,
    INST_ALL_OF,
    INST_WELLNESS_CHECK
} InstructionType;

typedef struct {
    InstructionType type;
    int arg_count;
    int args[4];
} Instruction;

Instruction* parse_schedule(const char *schedule_str, int *out_count);

#endif