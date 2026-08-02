#ifndef SECOND_PASS_H
#define SECOND_PASS_H

/**
 * @file second_pass.h
 * @brief Executes the second pass of the assembly process
 * It uses the built symbol table from the first pass to encode the instructions to binary
 * It resolves memory addresses for jumps and branches, flags .entry directives and tracks external symbols
 * @author Shaked Pollak, Daniela Aslan
 */

#include "globals.h"
#include "symbol_table.h"
#include "memory_image.h"
#include "ext_tracker.h"

/**
 * @fn run_second_pass
 * @brief This function reads the .am file line by line
 * It ignores directives except .entry which it uses to update the symbol table
 * For code instructions, it uses the symbol table to complete the binary encoding of what was left during the first pass
 * 
 * @param[in]   filename    The full name of the .am file to go through
 * @param[out]  sym_head    Pointer to the head of the symbol table linked list
 * @param[out]  code_image  Static array representing the instruction memory
 * @param[out]  ext_head    Pointer to the head of the externals linked list
 * @return                  An integer of status based on status_t enum
 */
int run_second_pass(const char* filename, symbol_node_t *sym_head, machine_word_t *code_image, ext_node_t **ext_head);

#endif /* SECOND_PASS_H */