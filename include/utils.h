#ifndef UTILS_H
#define UTILS_H

/**
 * @file utils.h
 * @brief Provides utility functions for file and memory management
 * @author Shaked Pollak, Daniela Aslan
 */

#include "globals.h"

#include <stdio.h>

/**
 * @fn open_file_with_extension
 * @brief Allocates memory for filename with extension,
 * appends the extention and open the file with the mode received
 * 
 * @param[in]   base_name  The string of filename without extension
 * @param[in]   extension  The string of the extension
 * @param[in]   mode       The string specifies on what mode should the file be opened
 * @param[out]  status     Pointer to a status_t enum
 * 
 * @return                Valid FILE pointer or NULL if memory allocation
 */
FILE* open_file_with_extension(const char* base_name, const char* extension, const char* mode, status_t *status, unsigned int asm_line_counter);

/**
 * @fn validate_operands
 * @brief The function checks if the operands are valid - if there are only 1 comma between 2 operands
 * 
 * @param[in]  operands  A raw string of operands
 * @return               An integer of status based on status_t enum
 */
int validate_operands(const char* operands);

#endif /* UTILS_H */