#include "../include/second_pass.h"
#include "../include/parser.h"
#include "../include/instructions.h"
#include "../include/symbol_table.h"
#include "../include/ext_tracker.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENTRY_DIRECTIVE (".entry")

/**
 * @fn handle_entry
 * @brief This function handles the .entry directives
 * It goes through the symbol table for the label and marks its 'is_entry' flag as true
 * @param[in]      parsed    Pointer to the parsed line struct containing the lines details
 * @param[in,out]  sym_head  Pointer to the head of the symbol table linked list
 * @return                   An integer of status based on status_t enum
 */
static int handle_entry(parsed_line_t *parsed, symbol_node_t *sym_head);

/**
 * @fn encode_instruction
 * @brief This function acts as a dispatcher that calls the appropriate handler for each instruction type
 *  
 * @param[in]      parsed      Pointer to the parsed line struct containing the lines details
 * @param[in,out]  sym_head    Pointer to the head of the symbol table linked list
 * @param[in,out]  code_image  Static array representing the instruction memory
 * @param[in,out]  ext_head    Pointer to the head of the externals linked list
 * @param[in,out]  IC          Pointer to a integer of the current Instruction Counter
 * @return                     An integer of status based on status_t enum
 */
static int encode_instruction(parsed_line_t *parsed, symbol_node_t *sym_head, machine_word_t *code_image, ext_node_t **ext_head, int *IC);

/**
 * @fn encode_r_type
 * @brief This function encodes the operands of an R-type instruction to the machine word
 * It parses up to 3 register numbers from the operand string
 * Acts upon 3-operand arithmetic and 2-operand copy instructions
 * 
 * @param[in]      operands            String containing raw operands
 * @param[in]      instruction_info    Pointer to the instructions data
 * @param[in,out]  word                Pointer to a machine word to update
 * @return                             An integer of status based on status_t enum
 */
static int encode_r_type(char* operands, const instruction_info_t *instruction_info, machine_word_t *word);

/**
 * @fn encode_i_type
 * @brief This function encodes the operands of an I-type instruction to the machine word
 * It handles conditional branches and standard arithmetic / load / store operations
 * 
 * @param[in]      operands            String containing raw operands
 * @param[in]      instruction_info    Pointer to the instructions data
 * @param[in,out]  word                Pointer to a machine word to update
 * @return                             An integer of status based on status_t enum
 */
static int encode_i_type(char* operands, const instruction_info_t *instruction_info, machine_word_t *word, symbol_node_t *sym_head, int curr_IC);

/**
 * @fn encode_j_type
 * @brief This function encodes the operands of an J-type instruction to the machine word
 * It resolves direct label addresses or register jumps
 * If an external symbol is used, it assigns to it the address 0 and logs the usage for the .ext file
 * 
 * @param[in]      operands            String containing raw operands
 * @param[in]      instruction_info    Pointer to the instructions data
 * @param[in,out]  word                Pointer to a machine word to update
 * @param[in]      sym_head            Pointer to the head of the symbol table
 * @param[in,out]  ext_head            Pointer the head of the external nodes linked list
 * @param[in]      curr_IC             The currenct instruction counter
 * @return                             An integer of status based on status_t enum
 */
static int encode_j_type(char* operands, const instruction_info_t *instruction_info, machine_word_t *word,
                            symbol_node_t *sym_head, ext_node_t **ext_head, int curr_IC);

int run_second_pass(const char* filename, symbol_node_t *sym_head, machine_word_t *code_image, ext_node_t **ext_head) {
    FILE *am_fptr = NULL;
    char line_buffer[MAX_LINE_LEN];
    char *am_filename = NULL;
    parsed_line_t parsed_line;
    status_t status = STATUS_UNINITIALIZED;
    int IC  = IC_START_ADDR;

    am_filename = (char*)malloc(strlen(filename) + FILE_EXTENTION_SIZE);

    if (am_filename == NULL) {
        fprintf(stderr, "Couldn't allocate memory for .as file extention\n");
        status = STATUS_FAILURE_MEMORY_ALLOCATION;
        goto lb_cleanup;
    }

    sprintf(am_filename, "%s.am", filename);

    am_fptr = fopen(am_filename, "r");
    if (am_fptr == NULL) {
        fprintf(stderr, "Couldn't open file %s in read mode\n", filename);
        status = STATUS_FAILURE_FILE_MGMT;
        goto lb_cleanup;
    }

    while (fgets(line_buffer, sizeof(line_buffer), am_fptr) != NULL) {
        memset(&parsed_line, 0, sizeof(parsed_line));

        if (parse_line(line_buffer, &parsed_line) != STATUS_SUCCESS)
            continue;
        if (parsed_line.operation[0] == '\0')
            continue;

        if (strcmp(parsed_line.operation, ENTRY_DIRECTIVE) == 0) {
            status = handle_entry(&parsed_line, sym_head);
            if (status != STATUS_SUCCESS) {
                goto lb_cleanup;
            }
        }
        else if (parsed_line.operation[0] != '.') {
            status = encode_instruction(&parsed_line, sym_head, code_image, ext_head, &IC);
            if (status != STATUS_SUCCESS) {
                goto lb_cleanup;
            }
        }
    }

    status = STATUS_SUCCESS;

lb_cleanup:
FREE_VAR(am_filename);
CLOSE_FILE(am_fptr);
return (int)status;
}

static int handle_entry(parsed_line_t *parsed, symbol_node_t *sym_head) {
    status_t status = STATUS_UNINITIALIZED;
    symbol_node_t *existing_sym = NULL;
    existing_sym = find_symbol(sym_head, parsed->operands);
    
    if (existing_sym == NULL) {
        fprintf(stderr, ".entry directive refers to undefined label '%s'\n", parsed->operands);
        status = STATUS_FAILURE_UNDEFINED_LABEL;
        goto lb_cleanup;
    }

    existing_sym->is_entry = 1;
    status = STATUS_SUCCESS;

lb_cleanup:
return (int)status;
}

static int encode_instruction(parsed_line_t *parsed, symbol_node_t *sym_head, machine_word_t *code_image, ext_node_t **ext_head, int *IC) {
    status_t status = STATUS_UNINITIALIZED;
    const instruction_info_t *instruction_info = NULL;
    int arr_idx = (*IC - IC_START_ADDR) / INSTRUCTION_SIZE_BYTES;

    instruction_info = get_instruction_info(parsed->operation);
    if (instruction_info == NULL) {
        fprintf(stderr, "Unknown instruction '%s'\n", parsed->operation);
        status = STATUS_FAILURE_UNKNOWN_OPERATION;
        goto lb_cleanup;
    }
    if (instruction_info->type == R_TYPE) {
        status = encode_r_type(parsed->operands, instruction_info, &code_image[arr_idx]);
    }
    else if (instruction_info->type == I_TYPE) {
        status = encode_i_type(parsed->operands, instruction_info, &code_image[arr_idx], sym_head, *IC);
    }
    else if (instruction_info->type == J_TYPE) {
        status = encode_j_type(parsed->operands, instruction_info, &code_image[arr_idx], sym_head, ext_head, *IC);
    }

    if (status == STATUS_SUCCESS) {
        *IC += INSTRUCTION_SIZE_BYTES;
    }

lb_cleanup:
return (int)status;
}

static int encode_r_type(char* operands, const instruction_info_t *instruction_info, machine_word_t *word) {
    char operands_cpy[MAX_LINE_LEN] = { 0 };
    char *token = NULL;
    int r1 = 0, r2 = 0, r3 = 0;
    int count = 0;

    strcpy(operands_cpy, operands);
    token = strtok(operands_cpy, ", \t\r\n");
    while (token != NULL && count < 3) {
        if (token[0] == '$') {
            if (count == 0)
                r1 = atoi(token + 1);
            else if (count == 1)
                r2 = atoi(token + 1);
            else if (count == 2)
                r3 = atoi(token + 1);

            count++;
        }
        token = strtok(NULL, ", \t\r\n");
    }
    
    if (instruction_info->opcode == 0) {
        /* 3-operand arithmetic instruction */
        word->r.rs = r1;
        word->r.rt = r2;
        word->r.rd = r3;
    }
    else if (instruction_info->opcode == 1) {
        /* 2-operand copy instruction */
        word->r.rs = r1;
        word->r.rt = r2;
        word->r.rd = 0;
    }

    return (int)STATUS_SUCCESS;
}

static int encode_i_type(char* operands, const instruction_info_t *instruction_info, machine_word_t *word, symbol_node_t *sym_head, int curr_IC) {
    status_t status = STATUS_UNINITIALIZED;
    char operands_cpy[MAX_LINE_LEN] = { 0 };
    char *t1 = NULL, *t2 = NULL, *t3 = NULL;
    symbol_node_t *sym = NULL;

    strcpy(operands_cpy, operands);
    t1 = strtok(operands_cpy, ", \t\r\n");
    t2 = strtok(NULL, ", \t\r\n");
    t3 = strtok(NULL, ", \t\r\n");

    if (t1 == NULL || t2 == NULL || t3 == NULL) {
        fprintf(stderr, "Missing operands for I-type instruction\n");
        status = STATUS_FAILURE_MISSING_OPERANDS;
        goto lb_cleanup;
    }

    if (instruction_info->opcode >= 15 && instruction_info->opcode <= 18) {
        /* Conditional branch instructions */
        word->i.rs = atoi(t1 + 1);
        word->i.rt = atoi(t2 + 1);

        sym = find_symbol(sym_head, t3);
        if (sym == NULL) {
            fprintf(stderr, "Undefined label '%s' in branch instruction\n", t3);
            status = STATUS_FAILURE_UNDEFINED_LABEL;
            goto lb_cleanup;
        }
        if (sym->type == SYM_EXTERNAL) {
            fprintf(stderr, "Target branch '%s' can't be an external label\n", t3);
            status = STATUS_FAILURE_UNDEFINED_LABEL;
            goto lb_cleanup;
        }

        word->i.immed = sym->address - curr_IC;
    }
    else {
        /* Arithmetic and load/store instructions */
        word->i.rs = atoi(t1 + 1);
        word->i.immed = atoi(t2); /* Raw integer without '$' */
        word->i.rt = atoi(t3 + 1);
    }

    status = STATUS_SUCCESS;

lb_cleanup:
return (int)status;
}

static int encode_j_type(char* operands, const instruction_info_t *instruction_info, machine_word_t *word,
                            symbol_node_t *sym_head, ext_node_t **ext_head, int curr_IC) {
    status_t status = STATUS_UNINITIALIZED;
    char operands_cpy[MAX_LINE_LEN] = { 0 };
    char *token = NULL;
    symbol_node_t *sym = NULL;

    if (instruction_info->opcode == 63) {
        /* hlt takes 0 operands */
        word->j.address = 0;
        word->j.reg = 0;
        status = STATUS_SUCCESS;
        goto lb_cleanup;
    }

    strcpy(operands_cpy, operands);
    token = strtok(operands_cpy, " \t\r\n");
    if (token == NULL) {
        fprintf(stderr, "Missing operand for J-type instruction\n");
        status = STATUS_FAILURE_MISSING_OPERANDS;
        goto lb_cleanup;
    }

    if (token[0] == '$') {
        /* Target is a register*/
        word->j.reg = 1;
        word->j.address = atoi(token + 1);
    }
    else {
        /* Target is a label */
        word->j.reg = 0;
        sym = find_symbol(sym_head, token);

        if (sym == NULL) {
            fprintf(stderr, "Undefined label '%s' in branch instruction\n", token);
            status = STATUS_FAILURE_UNDEFINED_LABEL;
            goto lb_cleanup;
        }
        if (sym->type == SYM_EXTERNAL) {
            word->j.address = 0;
            add_ext_record(ext_head, sym->name, curr_IC);
        }
        else 
            word->j.address = sym->address;
    }

    status = STATUS_SUCCESS;

lb_cleanup:
return (int)status;
}