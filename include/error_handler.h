#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

/**
 * @file error_handler.h
 * @brief Provides easy and modular error handling functions
 * @author Shaked Pollak, Daniela Aslan
 */

#include <stdarg.h>

/**
 * @fn print_asm_error
 * @brief The function prints error to stderr based on the format and the line number in which there was an error
 * It's purpose is to act on assembly errors
 * @param[in]  err_line_num  The line number in which there was an error
 * @param[in]  format        The format that should be printed
 * @param[in]  ...           Optional args for format
 * @return
 */
void print_asm_error(unsigned int err_line_num, const char* format, ...);


/**
 * @fn print_sys_error
 * @brief The function prints error to stderr based on the format and the line number in which there was an error
 * It's purpose is to act on general system errors
 * @param[in]  err_line_num  The line number in which there was an error
 * @param[in]  format        The format that should be printed
 * @param[in]  ...           Optional args for format
 * @return
 */
void print_sys_error(const char* format, ...);

#endif /* ERROR_HANDLER_H */