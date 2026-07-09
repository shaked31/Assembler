#include "../include/pre_assembler.h"
#include "../include/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* for .as\0 and .am\0 */
#define FILE_EXTENTION_SIZE 4 

int run_pre_assembler(const char* filename) {
    FILE *as_fptr, *am_fptr;
    char *as_filename, *am_filename;
    char line_buffer[MAX_LINE_LEN];
    ParsedLine parsed_line;
    uint8_t is_in_macro;

    as_filename = (char*)malloc(strlen(filename) + FILE_EXTENTION_SIZE);
    am_filename = (char*)malloc(strlen(filename) + FILE_EXTENTION_SIZE);

    if (as_filename == NULL || am_filename == NULL) {
        fprintf(stderr, "Couldn't allocate memory for file extention");
        return EXIT_FAILURE;
    }

    sprintf(as_filename, "%s.as", filename);
    sprintf(am_filename, "%s.am", filename);

    as_fptr = fopen(as_filename, "r");
    if (as_fptr == NULL) {
        fprintf(stderr, "Couldn't open file %s in read mode\n", as_filename);
        free(as_filename);
        free(am_filename);
        return EXIT_FAILURE;
    }

    am_fptr = fopen(am_filename, "w");
    if (am_fptr == NULL) {
        fprintf(stderr, "Couldn't open file %s in write mode\n", am_filename);
        free(as_filename);
        free(am_filename);
        fclose(as_fptr);
        return EXIT_FAILURE;
    }

    while (fgets(line_buffer, sizeof(line_buffer), as_fptr) != NULL) {
        memset(&parsed_line, 0, sizeof(parsed_line));
        parse_line(line_buffer, &parsed_line);
        
    }
}