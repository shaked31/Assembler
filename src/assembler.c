#include "../include/parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char* argv[]) {
    int i; /* for the for loop */
    int line_counter = 1;
    char buffer[MAX_LINE_LEN] = { 0 };

    FILE* fptr = NULL;
    char* full_filename;
    ParsedLine parsed_line;

    int parse_res;

    if (argc < 2) {
        fprintf(stderr, "Usage: ./assembler <file1> <file2> ...\n");
        return EXIT_FAILURE;
    }
    
    for (i = 1 ; i < argc ; i++) {    
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
            memset(&parsed_line, 0, sizeof(parsed_line));
            parse_res = parse_line(buffer, &parsed_line);
            if (!is_empty_or_comment(buffer)) {
                if (parse_res == 0) {
                    printf("Line number %d\n", line_counter);
                    printf("Label: '%s'\n", parsed_line.label);
                    printf("Operation: '%s'\n", parsed_line.operation);
                    printf("Operands: '%s'\n", parsed_line.operands);
                }
                else {
                    fprintf(stderr, "Problem parsing line %d\n", line_counter);
                }
            }
            line_counter++;
        }
    
        fclose(fptr);
        free(full_filename);
    }

    return 0;
}