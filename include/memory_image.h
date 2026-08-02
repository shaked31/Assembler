#ifndef MEMORY_IMAGE_H
#define MEMORY_IMAGE_H

/**
 * @file memory_image.h
 * @brief Defines structs for the 3 types of instructions and one for machine word to transfer the instruction into binary
 * @author Shaked Pollak, Daniela Aslan
 */

typedef struct {
    unsigned int unused : 6;
    unsigned int funct : 5;
    unsigned int rd : 5;
    unsigned int rt : 5;
    unsigned int rs : 5;
    unsigned int opcode : 6;
} r_type_t;

typedef struct {
    int immed : 16; /* sigend because it's 2's complement */
    unsigned int rt : 5;
    unsigned int rs : 5;
    unsigned int opcode : 6; 
} i_type_t;

typedef struct {
    unsigned int address : 25;
    unsigned int reg : 1;
    unsigned int opcode : 6; 
} j_type_t;

/**
 * Machine word union
 * Each variable of this union contains 32 bits
 * Can write data to specific struct of choise (r/i/j type)
 * Then read the raw_data for the final binary
 */
typedef union {
    r_type_t r;
    i_type_t i;
    j_type_t j;
    unsigned int raw_word;
} machine_word_t;

#endif /* MEMORY_IMAGE_H */