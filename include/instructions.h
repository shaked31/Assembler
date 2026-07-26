#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

typedef enum instruction_type_e {
    UNKNOWN_TYPE,
    R_TYPE,
    I_TYPE,
    J_TYPE
} instruction_type_t;

typedef struct instruction_info_s {
    char* name;
    instruction_type_t type;
    int opcode;
    int funct;
} instruction_info_t;

/**
 * @fn get_instruction_info
 * @brief Gets an asm command by its name
 * @param[in]  name  The name of the operation
 * @return           A pointer to the instruction_info_t struct if found, NULL otherwise
 */
const instruction_info_t* get_instruction_info(const char* name);

#endif