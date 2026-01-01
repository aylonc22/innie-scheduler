#include "instruction.h"
#include "innie.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INSTRUCTIONS 128

/* ------------------------------------------------------------ */
/* Instruction name → enum */
/* ------------------------------------------------------------ */
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
    if (strcmp(word, "CONDITIONAL_ADD") == 0) return INST_CONDITIONAL_ADD;
    return INST_INVALID;
}

/* ------------------------------------------------------------ */
/* Trim whitespace */
/* ------------------------------------------------------------ */
static char *trim(char *str) {
    while (isspace(*str)) str++;
    char *end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) *end-- = '\0';
    return str;
}

/* ------------------------------------------------------------ */
/* Resolve innie name → pointer */
/* ------------------------------------------------------------ */
static Innie *find_innie(const char *name, Innie *innies, int count) {
    for (int i = 0; i < count; i++) {
        if (strcmp(innies[i].name, name) == 0)
            return &innies[i];
    }
    return NULL;
}

/* ------------------------------------------------------------ */
/* Parse "[A, B, C]" */
/* ------------------------------------------------------------ */
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

/* ------------------------------------------------------------ */
/* Parse schedule */
/* ------------------------------------------------------------ */
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

        /* ---------------- SIMPLE ARITHMETIC ---------------- */
        case INST_LOAD:
        case INST_ADD:
        case INST_MULTIPLY:

            if (args_str[0] == '[') {
                parse_innie_list(args_str, &instr.args[0], innies, innie_count);
                instr.arg_count = 1;
            }
            else if (isdigit(*args_str) || *args_str == '-') {
                instr.args[0].type = ARG_INT;
                sscanf(args_str, "%d", &instr.args[0].int_value);
                instr.arg_count = 1;
            }
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

        /* ---------------- SHIFT ---------------- */
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

        /* ---------------- CONDITIONAL_ADD ---------------- */
        case INST_CONDITIONAL_ADD: {
            // CONDITIONAL_ADD [X] IF A > ANY OF [B, C]

            char *if_pos = strstr(args_str, " IF ");
            if (!if_pos) {
                fprintf(stderr, "Missing IF in CONDITIONAL_ADD: %s\n", line);
                exit(1);
            }

            *if_pos = '\0';
            char *add_part = trim(args_str);
            char *cond_part = trim(if_pos + 4);

            parse_innie_list(add_part, &instr.args[0], innies, innie_count);
            instr.arg_count = 1;

            instr.cond = malloc(sizeof(Condition));

            char lhs[32], op[4];
            sscanf(cond_part, "%31s %3s", lhs, op);

            Innie *lhs_i = find_innie(lhs, innies, innie_count);
            if (!lhs_i) {
                fprintf(stderr, "Unknown innie in condition: %s\n", lhs);
                exit(1);
            }

            instr.cond->lhs.type = ARG_INNIE;
            instr.cond->lhs.innie = lhs_i;

            if (strcmp(op, ">") == 0) instr.cond->type = COND_GT;
            else if (strcmp(op, "<") == 0) instr.cond->type = COND_LT;
            else if (strcmp(op, "==") == 0) instr.cond->type = COND_EQ;
            else {
                fprintf(stderr, "Invalid condition operator: %s\n", op);
                exit(1);
            }

            char *rhs = strstr(cond_part, op) + strlen(op);
            rhs = trim(rhs);

            if (strncmp(rhs, "ANY OF", 6) == 0) {
                rhs = trim(rhs + 6);
                parse_innie_list(rhs, &instr.cond->rhs, innies, innie_count);
            }else if (isdigit(*rhs) || *rhs == '-') {
                    instr.cond->rhs.type = ARG_INT;
                    instr.cond->rhs.int_value = atoi(rhs);
            } else {
                Innie *r = find_innie(rhs, innies, innie_count);
                if (!r) {
                    fprintf(stderr, "Unknown RHS innie: %s\n", rhs);
                    exit(1);
                }
                instr.cond->rhs.type = ARG_INNIE;
                instr.cond->rhs.innie = r;
            }
            break;
        }

        /* ---------------- NO-ARG ---------------- */
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
