#include "../include/parser.h"
#include "../include/pre_assembler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char* argv[]) {
    int i;
    int pre_asm_res = 0;

    if (argc < 2) {
        fprintf(stderr, "Usage: ./assembler <file1> <file2> ...\n");
        return EXIT_FAILURE;
    }
    
    for (i = 1 ; i < argc ; i++) {    
        pre_asm_res = run_pre_assembler(argv[i]);
        if (pre_asm_res != 0) {
            fprintf(stderr, "Error pre assembling\n");
            return EXIT_FAILURE;
        }
    }

    return 0;
}