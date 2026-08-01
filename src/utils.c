#include "../include/utils.h"

#include <stdlib.h>
#include <string.h>

FILE* open_file_with_extension(const char* base_name, const char* extension, const char* mode, status_t *status) {
    FILE *fptr = NULL;
    char* full_filename = NULL;

    /* Allocate memory for filename + extension + '.' + '\0' */
    full_filename = (char*)malloc(strlen(base_name) + strlen(extension) + 2);

    if (full_filename == NULL) {
        fprintf(stderr, "Couldn't allocate memory for .%s file extention\n", extension);
        *status = STATUS_FAILURE_MEMORY_ALLOCATION;
        return NULL;
    }
    sprintf(full_filename, "%s.%s", base_name, extension);
    fptr = fopen(full_filename, mode);
        if (fptr == NULL) {
        fprintf(stderr, "Couldn't open file %s in %s mode\n", full_filename, mode);
        *status = STATUS_FAILURE_FILE_MGMT;
        free(full_filename);
        return NULL;
    }

    free(full_filename);
    return fptr;
}