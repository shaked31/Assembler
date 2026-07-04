#include "../include/parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    int i; /* for the for loop */
    char buffer[MAX_LINE_LEN] = { 0 };

    if (argc < 2) {
        fprintf(stderr, "Usage: ./assembler <file1> <file2> ...\n");
        return EXIT_FAILURE;
    }
    
    for (i = 1 ; i < argc ; i++) {
        FILE* fptr;
        char* full_filename;
        /* Allocate memory for '.as' and '\0' */
        full_filename = (char*)malloc(strlen(argv[i]) + 4);
        if (full_filename == NULL) {
            fprintf(stderr, "Couldn't allocate memory for file extention of %s\n", argv[i]);
            continue;
        }

        strcpy(full_filename, argv[i]);
        strcat(full_filename, ".as");

        fptr = fopen(full_filename, "r");
        if (fptr == NULL) {
            fprintf(stderr, "Couldn't open file %s\n", argv[i]);
            free(full_filename);
            continue;
        }

        while (fgets(buffer, sizeof(buffer), fptr) != NULL) {
            printf("%s\n", buffer);
        }
    
        fclose(fptr);
        free(full_filename);
    }

    return 0;
}