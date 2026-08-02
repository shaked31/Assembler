#include "../include/first_pass.h"
#include "../include/parser.h"
#include "../include/instructions.h"
#include "../include/symbol_table.h"
#include "../include/memory_image.h"
#include "../include/utils.h"
#include "../include/error_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define EXTERN_DIRECTIVE (".extern")
#define ENTRY_DIRECTIVE (".entry")
#define DB_DIRECTIVE (".db")
#define DH_DIRECTIVE (".dh")
#define DW_DIRECTIVE (".dw")
#define ASCIZ_DIRECTIVE (".asciz")

#define DATA_BYTE (1)
#define DATA_HALF_WORD (2)
#define DATA_WORD (4)

#define IS_DATA_DIRECTIVE(directive) ((strcmp(directive, EXTERN_DIRECTIVE) == 0 || strcmp(directive, ENTRY_DIRECTIVE) == 0 || \
                                        strcmp(directive, ASCIZ_DIRECTIVE) == 0 || strcmp(directive, DB_DIRECTIVE) == 0 || \
                                        strcmp(directive, DH_DIRECTIVE) == 0 || strcmp(directive, DW_DIRECTIVE) == 0) ? 0 : 1)

/**
 * @fn handle_directive
 * @brief The function processes a data or structural directive
 *        It handles directives like .extern, .entry, .db, .dh, .dw, and .asciz
 *        It registers labels into the symbol table linked list
 *        It increments the Data Counter (DC) according to the memory required
 * 
 * @param[in]      parsed        Pointer to the parsed line struct containing the lines details
 * @param[in,out]  sym_head      Pointer to the head of the symbol table linked list
 * @param[in,out]  DC            Pointer to a integer of the current Data Counter
 * @param[in]      asm_line_num  An integer of the line count of the assembly source file
 * @return                       An integer of status based on status_t enum
 */
static int handle_directive(parsed_line_t *parsed, symbol_node_t **sym_head, unsigned char *data_image, int *DC, unsigned int asm_line_num);

/**
 * @fn handle_code
 * @brief The function processes an assembly instruction
 *        It checks that the instruction exists
 *        It registers labels into the symbol table linked list as a code symbol
 *        It increments the Instruction Counter (IC) according to the memory required
 * 
 * @param[in]      parsed            Pointer to the parsed line struct containing the lines details
 * @param[in,out]  sym_head          Pointer to the head of the symbol table linked list
 * @param[in,out]  IC                Pointer to a integer of the current Instruction Counter
 * @param[in]      asm_line_num  An integer of the line count of the assembly source file
 * @return                           An integer of status based on status_t enum
 */
static int handle_code(parsed_line_t *parsed, symbol_node_t **sym_head, machine_word_t *code_image, int *IC, unsigned int asm_line_num);

/**
 * @fn update_data_symbols
 * @brief The function offsets the memory addresses of all data symbols
 * The machine stores data memory immediately after code memory
 * This function adds the final Instruction Counter (IC) value to the address of every symbol 
 * marked with the 'is_data' flag.
 * 
 * @param[in,out]  sym_head  Pointer to the head of the symbol table linked list
 * @param[in]      final_IC  The final calculated Instruction Counter value
 * @return                   An integer of status based on status_t enum
 */
static int update_data_symbols(symbol_node_t **sym_head, int final_IC);

int run_first_pass(const char* filename, symbol_node_t **sym_head,
                        machine_word_t *code_image, unsigned char *data_image, int *out_IC, int *out_DC) {
    status_t status = STATUS_UNINITIALIZED;
    FILE *am_fptr = NULL;
    char line_buffer[MAX_LINE_LEN];
    parsed_line_t parsed_line;
    unsigned int asm_line_counter = 1;

    int IC  = IC_START_ADDR;
    int DC  = DC_START_ADDR;

    am_fptr = open_file_with_extension(filename, "am", "r", &status, asm_line_counter);
    if (am_fptr == NULL) {
        goto lb_cleanup;
    }

    while (fgets(line_buffer, sizeof(line_buffer), am_fptr) != NULL) {
        memset(&parsed_line, 0, sizeof(parsed_line));

        if (parse_line(line_buffer, &parsed_line, asm_line_counter) != STATUS_SUCCESS) {
            continue;
        }

        if (parsed_line.operation[0] == '.') {
            status = (int)handle_directive(&parsed_line, sym_head, data_image, &DC, asm_line_counter);
        }
        else {
            status = (int)handle_code(&parsed_line, sym_head, code_image, &IC, asm_line_counter);
        }

        if (status != STATUS_SUCCESS) {
            goto lb_cleanup;
        }

        asm_line_counter++;
    }

    update_data_symbols(sym_head, IC);
    *out_IC = IC;
    *out_DC = DC;

    status = STATUS_SUCCESS;

lb_cleanup:
CLOSE_FILE(am_fptr);
return (int)status;
}

static int handle_directive(parsed_line_t *parsed, symbol_node_t **sym_head, unsigned char *data_image, int *DC, unsigned int asm_line_num) { 
    status_t status = STATUS_UNINITIALIZED;
    symbol_node_t *existing_sym = NULL;
    char *str_start, *str_end;
    char openrads_cpy[MAX_LINE_LEN] = { 0 };
    char *token = NULL;
    long value = 0;
    int i = 0;
    

    if (IS_DATA_DIRECTIVE(parsed->operation) != 0) {
        /* Unknown directive */
        print_asm_error(asm_line_num, "Unknown directive '%s'\n", parsed->operation);
        status = STATUS_FAILURE_UNKNOWN_OPERATION;
        goto lb_cleanup;
    }

    if (strcmp(parsed->operation, EXTERN_DIRECTIVE) == 0) {
        /* Register a .extern directive */
        existing_sym = find_symbol(*sym_head, parsed->operands);
        if (existing_sym != NULL && existing_sym->type != SYM_EXTERNAL) {
            print_asm_error(asm_line_num, "External symbol '%s' is already defined locally\n", parsed->operands);
            status = STATUS_FAILURE_DUPLICATE_EXTERNAL_SYM;
            goto lb_cleanup;
        }
        if (existing_sym == NULL)
            status = insert_symbol(sym_head, parsed->operands, 0, SYM_EXTERNAL, 0);
        
        if (status != STATUS_SUCCESS)
            goto lb_cleanup;
    }
    
    else if (strcmp(parsed->operation, ENTRY_DIRECTIVE) == 0) {
        /* Skip entry directives in the first pass
           Because the label is defined later in the file, and in the first pass, we don't know yet where it exists in the file */
        status = STATUS_SUCCESS;
        goto lb_cleanup;
    }

    if (parsed->label[0] != '\0') {
        /* Register a label for data directives */
        existing_sym = find_symbol(*sym_head, parsed->label);
        if (existing_sym != NULL) {
            print_asm_error(asm_line_num, "Label '%s' is already defined\n", parsed->label);
            status = STATUS_FAILURE_DUPLICATE_LABEL_DEF;
            goto lb_cleanup;
        }

        status = insert_symbol(sym_head, parsed->label, *DC, SYM_DATA, 0);
        if (status != STATUS_SUCCESS)
            goto lb_cleanup;
    }

    if (strcmp(parsed->operation, ASCIZ_DIRECTIVE) == 0) {
        str_start = strchr(parsed->operands, '"');
        if (str_start == NULL) {
            print_asm_error(asm_line_num, "Invalid string format in .asciz\n");
            status = STATUS_FAILURE_INVALID_STR_IN_ASCIZ;
            goto lb_cleanup;
        }
        str_end = strrchr(str_start + 1, '"');
        if (str_end == NULL || str_end == str_start) {
            print_asm_error(asm_line_num, "Invalid string format in .asciz\n");
            status = STATUS_FAILURE_INVALID_STR_IN_ASCIZ;
            goto lb_cleanup;
        }

        for (i = 1 ; (str_start + i) < str_end ; i++) {
            data_image[(*DC)++] = (unsigned char)str_start[i];
        }
        data_image[(*DC)++] = '\0';
        status = STATUS_SUCCESS;
        goto lb_cleanup;
    }

    /* The directive is .db/.dh/.dw */
    if ((status = validate_operands(parsed->operands))) {
        print_asm_error(asm_line_num, "Invalid format of operands\n");
        goto lb_cleanup;
    }
    strcpy(openrads_cpy, parsed->operands);
    token = strtok(openrads_cpy, ", \t\r\n");

    while (token != NULL) {
        value = atol(token);

        if (strcmp(parsed->operation, DB_DIRECTIVE) == 0) {
            data_image[*DC] = (unsigned char)(value & 0xFF);
            (*DC) += DATA_BYTE;
        }
        else if (strcmp(parsed->operation, DH_DIRECTIVE) == 0) {
            data_image[*DC] = (unsigned char)(value & 0xFF);
            data_image[*DC + 1] = (unsigned char)((value >> 8) & 0xFF);
            (*DC) += DATA_HALF_WORD;
        }
        else if (strcmp(parsed->operation, DW_DIRECTIVE) == 0) {
            data_image[*DC] = (unsigned char)(value & 0xFF);
            data_image[*DC + 1] = (unsigned char)((value >> 8) & 0xFF);
            data_image[*DC + 2] = (unsigned char)((value >> 16) & 0xFF);
            data_image[*DC + 3] = (unsigned char)((value >> 24) & 0xFF);
            (*DC) += DATA_WORD;
        }

        token = strtok(NULL, ", \t\r\n");
    }

    status = STATUS_SUCCESS;

lb_cleanup:
return (int)status;
}

static int handle_code(parsed_line_t *parsed, symbol_node_t **sym_head, machine_word_t *code_image, int *IC, unsigned int asm_line_num) {
    const instruction_info_t *instruction_info = NULL;
    symbol_node_t *existing_sym = NULL;
    machine_word_t curr_word = { 0 };
    int arr_idx = 0;
    status_t status = STATUS_UNINITIALIZED;

    if (parsed->label[0] != '\0') {
        existing_sym = find_symbol(*sym_head, parsed->label);
        if (existing_sym != NULL) {
            print_asm_error(asm_line_num, "Duplicate label definition '%s'\n", parsed->label);
            status = STATUS_FAILURE_DUPLICATE_LABEL_DEF;
            goto lb_cleanup;
        }

        status = insert_symbol(sym_head, parsed->label, *IC, SYM_CODE, 0);
        if (status != STATUS_SUCCESS) {
            goto lb_cleanup;
        }
    }
    if (parsed->operation[0] == '\0') {
        status = STATUS_SUCCESS;
        goto lb_cleanup;
    }
    
    instruction_info = get_instruction_info(parsed->operation);
    if (instruction_info == NULL) {
        print_asm_error(asm_line_num, "Unknown instruction '%s'\n", parsed->operation);
        status = STATUS_FAILURE_UNKNOWN_OPERATION;
        goto lb_cleanup;
    }

    curr_word.r.opcode = instruction_info->opcode;
    if (instruction_info->type == R_TYPE) {
        curr_word.r.funct = instruction_info->funct;
    }

    arr_idx = (*IC - IC_START_ADDR) / INSTRUCTION_SIZE_BYTES;
    code_image[arr_idx] = curr_word;

    *IC += INSTRUCTION_SIZE_BYTES;

    status = STATUS_SUCCESS;

lb_cleanup:
return (int)status;
}

static int update_data_symbols(symbol_node_t **sym_head, int final_IC) {
    status_t status = STATUS_UNINITIALIZED;
    symbol_node_t *curr = *sym_head;
    
    if (curr == NULL) {
        /* Symbols table is empty, nothing to update */
        status = STATUS_SUCCESS;
        goto lb_cleanup;
    }

    while (curr != NULL) {
        /* If the symbol is a data directive (.db, .dh, .dw, .asciz) */
        if (curr->type == SYM_DATA) {
            curr->address += final_IC;
        }
        curr = curr->next;
    }

    status = STATUS_SUCCESS;
    
lb_cleanup:
return (int)status;
}