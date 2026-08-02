#include "../include/error_handler.h"

#include <stdio.h>

void print_asm_error(unsigned int err_line_num, const char* format, ...) {
    va_list args;
    fprintf(stderr, "[ASM Err - Line %d]: ");

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

void print_sys_error(const char* format, ...) {
    va_list args;
    fprintf(stderr, "[Sys Error]: ");

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}