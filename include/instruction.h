#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct Innie Innie;

/* ---------------- Instruction Types ---------------- */

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
    INST_CONDITIONAL_ADD,
    INST_INVALID
} InstructionType;

/* ---------------- Argument Types ---------------- */

typedef enum {
    ARG_INT,
    ARG_INNIE,
    ARG_LIST
} ArgType;

typedef struct Arg {
    ArgType type;
    union {
        int int_value;
        Innie *innie;
        struct {
            Innie **innies;
            int count;
        } list;
    };
} Arg;

/* ---------------- Conditions ---------------- */

typedef enum {
    COND_GT,   // >
    COND_LT,   // <
    COND_EQ    // ==
} ConditionType;

typedef struct {
    ConditionType type;
    Arg lhs;
    Arg rhs;   // usually ARG_INNIE or ARG_LIST (ANY_OF / ALL_OF)
} Condition;

/* ---------------- Instruction ---------------- */

typedef struct {
    InstructionType type;
    int arg_count;        // <= 2 always
    Arg args[2];          // instruction operands
    Condition *cond;      // NULL unless CONDITIONAL_ADD
} Instruction;

/* ---------------- Parser API ---------------- */

Instruction* parse_schedule(
    const char *schedule,
    int *out_count,
    Innie *innies,
    int innie_count
);

#endif
