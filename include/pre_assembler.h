#ifndef PRE_ASSEMBLER_H
#define PRE_ASSEMBLER_H

/**
 * @file pre_assembler.h
 * @brief Handles the macro expansion phase of the assembler
 * It reads the .as file, stores macros definitions dynamically at a linked list
 * It generates a new .am file in which all the macros are expanded into their code blocks
 * @author Shaked Pollak, Daniela Aslan
 */

#include "globals.h"

/**
 * @fn run_pre_assembler
 * @brief This function executes the pre-assembler phase on a given assembly source file (.as).
 * Reads the file line by line.
 * Identifies macro definitions, stores their contents dynamically in a linked list. 
 * Generates new expanded assembly file (.am).
 * When macro calls are encountered in the normal code flow, they are replaced with their respective expanded content.
 * 
 * @param[in] filename  The base name of the source file (without the .as extension).
 * @return              EXIT_SUCCESS if the file was parsed and the .am file was generated successfully
 *                      or EXIT_FAILURE if memory allocation fails, 
 *                      or if the input/output files cannot be opened.
 */
int run_pre_assembler(const char* filename);

#endif /* PRE_ASSEMBLER_H */