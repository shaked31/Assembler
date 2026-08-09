#include "../include/utils.h"
#include "../include/error_handler.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

FILE* open_file_with_extension(const char* base_name, const char* extension, const char* mode, status_t *status) {
    FILE *fptr = NULL;
    char* full_filename = NULL;

    /* Allocate memory for filename + extension + '.' + '\0' */
    full_filename = (char*)malloc(strlen(base_name) + strlen(extension) + 2);

    if (full_filename == NULL) {
        print_sys_error("Couldn't allocate memory for '.%s' file extension\n", extension);
        *status = STATUS_FAILURE_MEMORY_ALLOCATION;
        return NULL;
    }
    sprintf(full_filename, "%s.%s", base_name, extension);
    fptr = fopen(full_filename, mode);
        if (fptr == NULL) {
        print_sys_error("Couldn't open file %s in '%s' mode\n", full_filename, mode);
        *status = STATUS_FAILURE_FILE_MGMT;
        free(full_filename);
        return NULL;
    }

    free(full_filename);
    return fptr;
}

int validate_operands(const char* operands) {
    status_t status = STATUS_UNINITIALIZED;
    const char *ptr = operands;
    const char *next = NULL;

    while (*ptr && isspace((unsigned char)*ptr)) {
        /* Skip spaces after a ',' */
        ptr++;
    }
    
    if (*ptr == ',') {
        /* If first ',' before operands, it's invalid */
        status = STATUS_FAILURE_INVALID_OPERANDS;
        goto lb_cleanup;
    }

    while (*ptr) {
        if (*ptr == ',') {
            next = ptr + 1;

            while (*next && isspace((unsigned char)*next)) {
                /* Skip spaces after a ',' */
                next++;
            }
            
            if (*next == ',' || *next == '\0') {
                /* If next non-space char is a ',' or null-terminator, it's invalid */
                status = STATUS_FAILURE_INVALID_OPERANDS;
                goto lb_cleanup;
            }
        }
        ptr++;
    }

    status = STATUS_SUCCESS;

lb_cleanup:
return (int)status;
}

void flush_buffer(FILE *stream) {
    int c;
    
    /* Clear the buffer char by char */
    while ((c = fgetc(stream)) != '\n' && c != EOF);
}

int parse_assembler_arg(char* arg) {
    status_t status = STATUS_UNINITIALIZED;
    size_t arg_len = strlen(arg);
    char *extension = NULL;

    if (arg_len < 3) {
        status = STATUS_FAILURE_INVALID_ARG;
        goto lb_cleanup;
    }
    
    extension = arg + arg_len - AS_FILE_EXTENSION_SIZE;

    if (strcmp(extension, ".as") != 0) {
        status = STATUS_FAILURE_INVALID_ARG;
        goto lb_cleanup;
    }

    arg[arg_len - AS_FILE_EXTENSION_SIZE] = '\0';

    status = STATUS_SUCCESS;

lb_cleanup:
return (int)status;
}

void remove_invalid_am_file(const char* base_filename) {
    char am_filename[MAX_LINE_LEN] = { 0 };
    sprintf(am_filename, "%s.am", base_filename);
    remove(am_filename);
}

int is_numeric(const char* str) {
    status_t status = STATUS_UNINITIALIZED;
    char *endptr;

    if (str == NULL || *str == '\0') {
        status = STATUS_FAILURE_INVALID_OPERANDS;
        goto lb_cleanup;
    }

    errno = 0;    
    strtol(str, &endptr, 10);

    if (errno == ERANGE || endptr == str) {
        /* Check for integer overflow / underflow*/
        status = STATUS_FAILURE_INVALID_OPERANDS;
        goto lb_cleanup;
    }

    while (*endptr != '\0') {
        /* Skip trailing whitespaces */
        if (!isspace((unsigned char)*endptr)) {
            status = STATUS_FAILURE_INVALID_OPERANDS;
            goto lb_cleanup;
        }
        endptr++;
    }

    status = STATUS_SUCCESS;

lb_cleanup:
return (int)status;
}