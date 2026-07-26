#include "../include/first_pass.h"
#include "../include/parser.h"
#include "../include/instructions.h"
#include "../include/symbol_table.h"
#include "../include/globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXTERN_DIRECTIVE ".extern"
#define ENTRY_DIRECTIVE ".entry"
#define DB_DIRECTIVE ".db"
#define DH_DIRECTIVE ".dh"
#define DW_DIRECTIVE ".dw"
#define ASCIZ_DIRECTIVE ".asciz"

static int handle_directive(parsed_line_t *parsed, symbol_node_t **sym_head, int *DC);
static int handle_code(parsed_line_t *parsed, symbol_node_t **sym_head, int *IC);
static int update_data_symbols(symbol_node_t **sym_head, int final_IC);

int run_first_pass(const char* filename, symbol_node_t **sym_head) {
    FILE *am_fptr = NULL;
    char line_buffer[MAX_LINE_LEN];
    parsed_line_t parsed_line;
    status_t status = STATUS_UNINITIALIZED;

    int IC  = IC_START_ADDR;
    int DC  = DC_START_ADDR;

    am_fptr = fopen(filename, "r");
    if (am_fptr == NULL) {
        fprintf(stderr, "Couldn't open file %s in write mode\n", filename);
        status = STATUS_FAILURE_FILE_MGMT;
        goto lb_cleanup;
    }

    while (fgets(line_buffer, sizeof(line_buffer), am_fptr) != NULL) {
        memset(&parsed_line, 0, sizeof(parsed_line));

        if (parse_line(line_buffer, &parsed_line) != STATUS_SUCCESS) {
            continue;
        }

        if (parsed_line.operation[0] == '.') {
            status = (int)handle_directive(&parsed_line, &DC, sym_head);
        }
        else {
            status = (int)handle_code(&parsed_line, &IC, sym_head);
        }
    }

    update_data_symbols(sym_head, IC);

    status = STATUS_SUCCESS;

lb_cleanup:
CLOSE_FILE(am_fptr);
return (int)status;
}

static int handle_directive(parsed_line_t *parsed, symbol_node_t **sym_head, int *DC) {
    symbol_node_t *existing_sym;
    int num_elements = 0;
    int data_size_multiplier = 0;
    char *str_start, *str_end;

    if (strcmp(parsed->operation, EXTERN_DIRECTIVE) == 0) {}
}

static int handle_code(parsed_line_t *parsed, symbol_node_t **sym_head, int *IC) {
    instruction_info_t *instruction_info = NULL;
    symbol_node_t *existing_sym = NULL;
    status_t status = STATUS_UNINITIALIZED;

    if (parsed->label[0] != '\0') {
        existing_sym = find_symbol(*sym_head, parsed->label);
        if (existing_sym != NULL) {
            fprintf(stderr, "Duplicate label definition '%s'\n", parsed->label);
            status = STATUS_FAILURE_DUPLICATE_LABEL_DEF;
            goto lb_cleanup;
        }

        status = insert_symbol(sym_head, parsed->label, *IC, 1, 0, 0, 0);
        if (status != STATUS_SUCCESS) {
            goto lb_cleanup;
        }
    }
    instruction_info = get_instruction_info(parsed->operation);
    if (instruction_info == NULL) {
        fprintf(stderr, "Unknown instruction '%s'\n", parsed->operation);
        status = STATUS_FAILURE_UNKNOWN_OPERATION;
        goto lb_cleanup;
    }

    *IC += INSTRUCTION_SIZE_BYTES;

    status = STATUS_SUCCESS;

lb_cleanup:
return (int)status;
}

static int update_data_symbols(symbol_node_t **sym_head, int final_IC) {

}