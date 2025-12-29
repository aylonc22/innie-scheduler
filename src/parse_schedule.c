#include "instruction.h"
#include "innie.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INSTRUCTIONS 128

// ------------------------------------------------------------
// Instruction name → enum
// ------------------------------------------------------------
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
    return INST_INVALID;
}

// ------------------------------------------------------------
// Trim whitespace
// ------------------------------------------------------------
static char *trim(char *str) {
    while (isspace(*str)) str++;
    char *end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) *end-- = '\0';
    return str;
}

// ------------------------------------------------------------
// Resolve innie name → pointer
// ------------------------------------------------------------
static Innie *find_innie(
    const char *name,
    Innie *innies,
    int innie_count
) {
    for (int i = 0; i < innie_count; i++) {
        if (strcmp(innies[i].name, name) == 0)
            return &innies[i];
    }
    return NULL;
}

// ------------------------------------------------------------
// Parse "[HELLY, MARK]"
// ------------------------------------------------------------
static void parse_innie_list(
    const char *text,
    Arg *arg,
    Innie *innies,
    int innie_count
) {
    arg->type = ARG_LIST;
    arg->list.count = 0;
    arg->list.innies = malloc(sizeof(Innie *) * innie_count);

    const char *p = text + 1; // skip '['
    char buf[64];
    int idx = 0;

    while (*p && *p != ']') {
        if (*p == ',' || isspace(*p)) {
            if (idx > 0) {
                buf[idx] = '\0';
                Innie *i = find_innie(buf, innies, innie_count);
                if (!i) {
                    fprintf(stderr, "Unknown innie: %s\n", buf);
                    exit(1);
                }
                arg->list.innies[arg->list.count++] = i;
                idx = 0;
            }
            p++;
            continue;
        }
        buf[idx++] = *p++;
    }

    if (idx > 0) {
        buf[idx] = '\0';
        Innie *i = find_innie(buf, innies, innie_count);
        if (!i) {
            fprintf(stderr, "Unknown innie: %s\n", buf);
            exit(1);
        }
        arg->list.innies[arg->list.count++] = i;
    }
}

// ------------------------------------------------------------
// Parse schedule
// ------------------------------------------------------------
Instruction *parse_schedule(
    const char *schedule_str,
    int *out_count,
    Innie *innies,
    int innie_count
) {
    Instruction *instructions =
        malloc(sizeof(Instruction) * MAX_INSTRUCTIONS);
    int count = 0;

    char *copy = strdup(schedule_str);
    char *line = strtok(copy, "\n");

    while (line) {
        line = trim(line);
        if (*line == '\0') {
            line = strtok(NULL, "\n");
            continue;
        }

        char instr_name[32];
        sscanf(line, "%31s", instr_name);

        InstructionType type = get_instruction_type(instr_name);
        if (type == INST_INVALID) {
            fprintf(stderr, "Unknown instruction: %s\n", instr_name);
            exit(1);
        }

        Instruction instr = {0};
        instr.type = type;

        char *args_str = trim(line + strlen(instr_name));

        switch (type) {

        case INST_LOAD:
        case INST_ADD:
        case INST_MULTIPLY:

            // list
            if (args_str[0] == '[') {
                parse_innie_list(args_str, &instr.args[0], innies, innie_count);
                instr.arg_count = 1;
            }
            // integer
            else if (isdigit(*args_str) || *args_str == '-') {
                instr.args[0].type = ARG_INT;
                if (sscanf(args_str, "%d",
                        &instr.args[0].int_value) != 1) {
                    fprintf(stderr, "Invalid number: %s\n", line);
                    exit(1);
                }
                instr.arg_count = 1;
            }
            // single innie
            else {
                Innie *i = find_innie(args_str, innies, innie_count);
                if (!i) {
                    fprintf(stderr, "Unknown innie: %s\n", args_str);
                    exit(1);
                }
                instr.args[0].type = ARG_INNIE;
                instr.args[0].innie = i;
                instr.arg_count = 1;
            }
            break;

        case INST_SHIFT: {
            int times;
            if (sscanf(args_str, "%d TIMES", &times) != 1) {
                fprintf(stderr, "Invalid SHIFT: %s\n", line);
                exit(1);
            }
            instr.args[0].type = ARG_INT;
            instr.args[0].int_value = times;
            instr.arg_count = 1;
            break;
        }

        case INST_WAFFLE:
        case INST_END_SHIFT:
            instr.arg_count = 0;
            break;

        default:
            break;
        }

        instructions[count++] = instr;
        if (count >= MAX_INSTRUCTIONS) {
            fprintf(stderr, "Too many instructions\n");
            exit(1);
        }

        line = strtok(NULL, "\n");
    }

    free(copy);
    *out_count = count;
    return instructions;
}
