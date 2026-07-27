#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "globals.h"

/**
 * @fn run_first_pass
 * @brief Executes the first pass of the assembly process.
 *        This function reads the .am file line by line.
 *        It decides whether each line is a directive or code instruction
 *        It builds the inital symbol table (without .entry) and calculates the memory needed for code (IC) and data (DC)
 * @param[in]  filename  The full name of the .am file to go through
 * @param[out] sym_head  Pointer to the head of the symbol table linked list
 * @return               An integer of status based on status_t enum
 */
int run_first_pass(const char* filename, symbol_node_t **sym_head);

#endif