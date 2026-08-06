#include "../include/utils.h"
#include "../include/error_handler.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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