#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdlib.h>

typedef struct Innie Innie;

typedef enum {
    INST_LOAD,
    INST_ADD,
    INST_MULTIPLY,
    INST_WAFFLE,
    INST_SHIFT,
    INST_END_SHIFT,
    INST_ANY_OF,
    INST_ALL_OF,
    INST_WELLNESS_CHECK,
    INST_INVALID
} InstructionType;

typedef enum {
    ARG_INT,
    ARG_INNIE,
    ARG_LIST
} ArgType;

typedef struct Arg {
    ArgType type;
    union {
        int int_value;
        struct Innie *innie;      
        struct {                  
            Innie **innies;
            int count;
        } list;
    };
} Arg;

typedef struct {
    InstructionType type;
    int arg_count;
    Arg args[2];
} Instruction;

Instruction* parse_schedule( const char *schedule,
    int *out_count,
    Innie *innies,
    int innie_count);

#endif
