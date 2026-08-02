/**
 * @file assembler.c
 * @brief This source file is the main entry point of the assembler program
 * It receives CLI arguments for target files, initialize memory data structures
 * It uses the main functions of pre_assember.h, first_pass.h, second_pass.h to complete the assembler program
 * It handles memory cleanup
 * @author Shaked Pollak, Daniela Aslan
 */


#include "../include/parser.h"
#include "../include/pre_assembler.h"
#include "../include/first_pass.h"
#include "../include/second_pass.h"
#include "../include/symbol_table.h"
#include "../include/memory_image.h"
#include "../include/file_generator.h"
#include "../include/globals.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char* argv[]) {
    status_t status = STATUS_UNINITIALIZED;
    symbol_node_t *sym_head = NULL;
    ext_node_t *ext_head = NULL;
    machine_word_t code_image[MAX_MEMORY_SIZE] = { 0 };
    unsigned char data_image[MAX_MEMORY_SIZE] = { 0 };
    int IC, DC;
    int i = 0;

    if (argc < 2) {
        fprintf(stderr, "Usage: ./assembler <file1> <file2> ...\n");
        status = STATUS_FAILURE_BINARY_USAGE;
        return status;
    }
    
    for (i = 1 ; i < argc ; i++) {
        if (i != 1) printf("\n");
        printf("Assembling file %s\n", argv[i]);

        sym_head = NULL;
        ext_head = NULL;
        memset(code_image, 0, sizeof(code_image));
        memset(data_image, 0, sizeof(data_image));
        
        if ((status = run_pre_assembler(argv[i]))) {
            fprintf(stderr, "Error pre assembling file %s\n", argv[i]);
            continue;
        }

        if ((status = run_first_pass(argv[i], &sym_head, code_image, data_image, &IC, &DC))) {
            fprintf(stderr, "Error in first pass for file %s\n", argv[i]);
            free_symbol_table(sym_head);
            continue;
        }

        if ((status = run_second_pass(argv[i], sym_head, code_image, &ext_head))) {
            fprintf(stderr, "Error in second pass for file %s\n", argv[i]);
            free_symbol_table(sym_head);
            free_ext_list(ext_head);
            continue;
        }

        if ((status = generate_files(argv[i], code_image, data_image, &sym_head, &ext_head, IC, DC))) {
            fprintf(stderr, "Error generating output files for file %s\n", argv[i]);
            free_symbol_table(sym_head);
            free_ext_list(ext_head);
            continue;
        }

        printf("Successfully assembled %s\n", argv[i]);


        free_symbol_table(sym_head);
        free_ext_list(ext_head);
    }

    return (int)status;
}