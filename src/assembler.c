#include "../include/parser.h"
#include "../include/pre_assembler.h"
#include "../include/first_pass.h"
#include "../include/symbol_table.h"
#include "../include/globals.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char* argv[]) {
    int i = 0;
    symbol_node_t *sym_head = NULL;

    if (argc < 2) {
        fprintf(stderr, "Usage: ./assembler <file1> <file2> ...\n");
        return EXIT_FAILURE;
    }
    
    for (i = 1 ; i < argc ; i++) {    
        if (run_pre_assembler(argv[i]) != STATUS_SUCCESS) {
            fprintf(stderr, "Error pre assembling\n");
            return EXIT_FAILURE;
        }

        if (run_first_pass(argv[i], &sym_head) != STATUS_SUCCESS) {
            fprintf(stderr, "Error in first pass\n");
            free_symbol_table(sym_head);
            return EXIT_FAILURE;
        }
    }

    free_symbol_table(sym_head);
    return 0;
}