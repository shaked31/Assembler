#include "../include/instructions.h"

#include <string.h>

#define INSTRUCTIONS_NUM (sizeof(instruction_table) / sizeof(instruction_table[0]))

static const instruction_info_t instruction_table[] = {
    /* R-Type Instructions */
    {"add",  R_TYPE, 0, 1},
    {"sub",  R_TYPE, 0, 2},
    {"and",  R_TYPE, 0, 3},
    {"or",   R_TYPE, 0, 4},
    {"nor",  R_TYPE, 0, 5},
    {"move", R_TYPE, 1, 1},
    {"mvhi", R_TYPE, 1, 2},
    {"mvlo", R_TYPE, 1, 3},

    /* I-Type Instructions */
    {"addi", I_TYPE, 10, 0},
    {"subi", I_TYPE, 11, 0},
    {"andi", I_TYPE, 12, 0},
    {"ori",  I_TYPE, 13, 0},
    {"nori", I_TYPE, 14, 0},
    {"bne",  I_TYPE, 15, 0},
    {"beq",  I_TYPE, 16, 0},
    {"blt",  I_TYPE, 17, 0},
    {"bgt",  I_TYPE, 18, 0},
    {"lb",   I_TYPE, 19, 0},
    {"sb",   I_TYPE, 20, 0},
    {"lw",   I_TYPE, 21, 0},
    {"sw",   I_TYPE, 22, 0},
    {"lh",   I_TYPE, 23, 0},
    {"sh",   I_TYPE, 24, 0},

    /* J-Type Instructions */
    {"jmp",  J_TYPE, 30, 0},
    {"la",   J_TYPE, 31, 0},
    {"call", J_TYPE, 32, 0},
    {"hlt",  J_TYPE, 63, 0}
};

const instruction_info_t* get_instruction_info(const char* name) {
    int i = 0;
    for (i = 0 ; i < INSTRUCTIONS_NUM ; i++) {
        if (strcmp(instruction_table[i].name, name) == 0) {
            return &instruction_table[i];
        }
    }
    return NULL;
}