#ifndef UTILS_H
#define UTILS_H

/**
 * @file utils.h
 * @brief This module provides utility functions for file and memory management
 * 
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
 * @return                Valid FILE pointer or NULL if memory allocation
 */
FILE* open_file_with_extension(const char* base_name, const char* extension, const char* mode, status_t *status);

#endif /* UTILS_H */