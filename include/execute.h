#ifndef EXECUTE_H
#define EXECUTE_H

#include "innie.h"

typedef enum {
    EXEC_OK,
    EXEC_BLOCKED,
    EXEC_TERMINATE
} ExecResult;

ExecResult execute_instruction(Innie *innie, Instruction *instr);

#endif
