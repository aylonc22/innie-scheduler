#include "instruction.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INSTRUCTIONS 64 

static InstructionType get_instruction_type(const char *word) {
    if (strcmp(word, "LOAD") == 0) return INST_LOAD;
    if (strcmp(word, "ADD") == 0) return INST_ADD;
    if (strcmp(word, "MULTIPLY") == 0) return INST_MULTIPLY;
    if (strcmp(word, "WAFFLE") == 0) return INST_WAFFLE;
    if (strcmp(word, "SHIFT") == 0) return INST_SHIFT;
    if (strcmp(word, "END_SHIFT") == 0) return INST_END_SHIFT;
    if (strcmp(word, "ANY_OF") == 0) return INST_ANY_OF;
    if (strcmp(word, "ALL_OF") == 0) return INST_ALL_OF;
    if (strcmp(word, "WELLNESS_CHECK") == 0) return INST_WELLNESS_CHECK;
    return -1;
}

Instruction* parse_schedule(const char *schedule_str, int *out_count) {
    Instruction *instructions = malloc(sizeof(Instruction) * MAX_INSTRUCTIONS);
    int count = 0;

    char *sched_copy = strdup(schedule_str);
    char *line = strtok(sched_copy, "\n");

    while (line) {
        // skip empty lines
        while (isspace(*line)) line++;
        if (*line == '\0') {
            line = strtok(NULL, "\n");
            continue;
        }

        char instr_name[32];
        int args[4] = {0};
        int arg_count = 0;

        // try to parse first word and optional arguments
        int scanned = sscanf(line, "%31s %d %d %d %d", instr_name, &args[0], &args[1], &args[2], &args[3]);
        if (scanned < 1) {
            fprintf(stderr, "Failed to parse line: %s\n", line);
            exit(1);
        }

        InstructionType type = get_instruction_type(instr_name);
        if (type == -1) {
            fprintf(stderr, "Unknown instruction: %s\n", instr_name);
            exit(1);
        }

        if (type == INST_LOAD || type == INST_ADD || type == INST_MULTIPLY) {
            arg_count = 1; // for now, assume only one integer arg
        } else {
            arg_count = 0; // no args
        }

        // store instruction
        instructions[count].type = type;
        instructions[count].arg_count = arg_count;
        for (int i = 0; i < arg_count; i++)
            instructions[count].args[i] = args[i];

        count++;
        line = strtok(NULL, "\n");
    }

    free(sched_copy);
    *out_count = count;
    return instructions;
}
