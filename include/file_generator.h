#ifndef FILE_GENERATOR_H
#define FILE_GENERATOR_H

/**
 * @file file_generator.h
 * @brief This module handles the generation of the output files for the assembler
 * It takes the compiled code and data images and symbol tables and exports them to:
 *    1. '.ob' file - the binary itself
 *    2. '.ent' file - including entry symbols and its address
 *    3. '.ext' file - including external symbols and its address
 * @author Shaked Pollak, Daniela Aslan
 */

#include "globals.h"
#include "memory_image.h"
#include "symbol_table.h"
#include "ext_tracker.h"

/**
 * @fn generate_files
 * @brief This function generates the .ob, .ent, .ext output files for the assembled program
 * 
 * @param[in]      filename    The full name of the .am file to go through
 * @param[out]     code_image  Array representing the instruction memory
 * @param[out]     data_image  Array representing the data memory
 * @param[out]     sym_head    Pointer to the head of the symbol table linked list
 * @param[out]     ext_head    Pointer to the head of the externals linked list
 * @param[in,out]  IC          Pointer to an integer of the current instruction counter
 * @param[in,out]  DC          Pointer to an integer of the current data counter
 * @return                  An integer of status based on status_t enum
 */
int generate_files(const char* filename, machine_word_t *code_image, unsigned char *data_image,
                        symbol_node_t **sym_head, ext_node_t **ext_head, int IC, int DC, unsigned int line_counter);

#endif /* FILE_GENERATOR_H */