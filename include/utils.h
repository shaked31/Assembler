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
FILE* open_file_with_extension(const char* base_name, const char* extension, const char* mode, status_t *status);

/**
 * @fn validate_operands
 * @brief The function checks if the operands are valid - if there are only 1 comma between 2 operands
 * 
 * @param[in]  operands  A raw string of operands
 * @return               An integer of status based on status_t enum
 */
int validate_operands(const char* operands);

/**
 * @fn flush_buffer
 * @brief Clears the buffer of the stream
 * 
 * @param[in]  stream  Pointer to the stream to clear
 * @return               An integer of status based on status_t enum
 */
void flush_buffer(FILE *stream);

/**
 * @fn parse_assembler_arg
 * @brief checks validity of argument, changes if if able
 * 
 * @param[in,out]  arg  String of argument from main function
 * @return              An integer of status based on status_t enum
 */
int parse_assembler_arg(char* arg);

/**
 * @fn remove_invalid_am_file
 * @brief Deletes .am file based on received filename
 * 
 * @param[in]  base_filename  The base filename
 */
void remove_invalid_am_file(const char* base_filename);

/**
 * @fn is_numeric
 * @brief Checks if a string is numeric
 * 
 * @param[in]  str  The string to check
 * @return          1 if the string is numeric, else 0
 */
int is_numeric(const char* str);

#endif /* UTILS_H */