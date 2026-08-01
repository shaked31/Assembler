#ifndef FIRST_PASS_H
#define FIRST_PASS_H

/**
 * @file first_pass.h
 * @brief This module executes the first pass of the assembly process
 * It reads the new .am file and builds a symbol table from it
 * It counts the required memory for instructions and data
 * It leaves memory addresses for .entry labels to be handled in the second pass
 * 
 * @author Shaked Pollak, Daniela Aslan
 */

#include "globals.h"
#include "memory_image.h"

/**
 * @fn run_first_pass
 * @brief This function reads the .am file line by line
 * It decides whether each line is a directive or code instruction
 * It builds the inital symbol table (without .entry) and calculates the memory needed for code (IC) and data (DC)
 * 
 * @param[in]   filename    The full name of the .am file to go through
 * @param[out]  sym_head    Pointer to the head of the symbol table linked list
 * @param[out]  code_image  Static array representing the instruction memory
 * @param[out]  data_image  Static array representing the data memory
 * @param[out]  IC          Pointer to store the final instruction counter
 * @param[out]  DC          Pointer to store the final data counter
 * @return                  An integer of status based on status_t enum
 */
int run_first_pass(const char* filename, symbol_node_t **sym_head,
                        machine_word_t *code_image, unsigned char *data_image, int *IC, int *DC);
                        
#endif /* FIRST_PASS_H */