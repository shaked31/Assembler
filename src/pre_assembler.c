#include "../include/globals.h"
#include "../include/pre_assembler.h"
#include "../include/parser.h"
#include "../include/symbol_table.h"
#include "../include/memory_image.h"
#include "../include/instructions.h"
#include "../include/utils.h"
#include "../include/error_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @fn append_to_macro
 * @brief Helper function to append a line of text to a macro's content
 * 
 * This function operates differently and accordingly if the macro is new.
 * If it's already define and then it adds data to it using realloc and strncat
 * 
 * @param[in]  macro  Pointer to a macro_node_t struct
 * @param[in]  line   String of the current line from the .as file
 * @return            An integer of success / failure.
 */
static int append_to_macro(macro_node_t *macro, const char* line);

/**
 * @fn free_macro_table
 * @brief This function safely frees all dynamically allocated memory in the macro table.
 * it iterates through the linked list, freeing the content strings first,
 * and then freeing the nodes themselves to prevent memory leaks.
 * @param[in]  head  Pointer to the head of the macros linked list.
 */
static void free_macro_table(macro_node_t *head);

/**
 * @fn find_macro
 * @brief Searches the macro linked list for a specific macro by name.
 * @param[in]  head  Pointer to the head of the macros linked list.
 * @param[in]  name  The name of the macro to search for.
 * @return           Pointer to the macros if found, or NULL if not found.
 */
static macro_node_t* find_macro(macro_node_t *head, const char* name);

int run_pre_assembler(const char* filename) {
    status_t status = STATUS_UNINITIALIZED, final_status = STATUS_SUCCESS;
    FILE *as_fptr = NULL, *am_fptr = NULL;
    char line_buffer[MAX_LINE_LEN];
    parsed_line_t parsed_line = { 0 };
    macro_node_t *curr_mcro = NULL, *head_mcro = NULL, *found_mcro = NULL, *new_node = NULL;
    unsigned char is_in_macro = 0;
    unsigned int asm_line_counter = 1;

    as_fptr = open_file_with_extension(filename, "as", "r", &status);
    if (as_fptr == NULL) {
        goto lb_cleanup;
    }

    am_fptr = open_file_with_extension(filename, "am", "w", &status);
    if (am_fptr == NULL) {
        goto lb_cleanup;
    }

    while (fgets(line_buffer, sizeof(line_buffer), as_fptr) != NULL) {
        memset(&parsed_line, 0, sizeof(parsed_line));
        status = parse_line(line_buffer, &parsed_line, asm_line_counter);

        if (status == STATUS_FAILURE_LINE_TOO_LONG && !feof(as_fptr)) {
            flush_buffer(as_fptr);
        }

        if (status != STATUS_SUCCESS) {
            if (status != STATUS_FAILURE_NOTHING_TO_PARSE)
                final_status = status;
            asm_line_counter++;
            continue;
        }

        if (is_in_macro) {
            if (strcmp(parsed_line.operation, "mcroend") == 0) {
                is_in_macro = 0;
                curr_mcro = NULL;
            }
            else {
                if ((status = (status_t)append_to_macro(curr_mcro, line_buffer))) {
                    final_status = status;
                }
            }
        }

        else {
            if (strcmp(parsed_line.operation, "mcro") == 0) {
                /* Found a new macro definition */
                if(get_instruction_info(parsed_line.operands) != NULL) {
                    /* The macro name is an instruction name */
                    print_asm_error(asm_line_counter, "Macro name '%s' can't be an instruction name\n", parsed_line.operation);
                    final_status = STATUS_FAILURE_INVALID_MACRO_NAME;
                    asm_line_counter++;
                    continue;
                }

                new_node = (macro_node_t*)malloc(sizeof(macro_node_t));
                if (new_node == NULL) {
                    print_sys_error("Couldn't allocate memory for a new macro node\n");
                    status = STATUS_FAILURE_MEMORY_ALLOCATION;
                    goto lb_cleanup;
                } 
                strcpy(new_node->name, parsed_line.operands);

                /* Insert the new macro in the start of the linked list */
                new_node->content = NULL;
                new_node->next = head_mcro;
                head_mcro = new_node;
                curr_mcro = new_node;
                is_in_macro = 1;
            }
            
            else {
                found_mcro = find_macro(head_mcro, parsed_line.operation);
                if (found_mcro != NULL) {
                    if (parsed_line.label[0] != '\0') {
                        fprintf(am_fptr, "%s:\n", parsed_line.label);
                    }
                    fputs(found_mcro->content, am_fptr);
                }
                else {
                    /* Just a normal assembly operation (like add) */
                    fputs(line_buffer, am_fptr);
                }
            }
        }
        asm_line_counter++;
    }
    
    if (status != STATUS_SUCCESS) {
        status = final_status;
        goto lb_cleanup;
    }

    status = STATUS_SUCCESS;

lb_cleanup:
CLOSE_FILE(as_fptr);
CLOSE_FILE(am_fptr);
free_macro_table(head_mcro); /* Contains null checking */
return (int)status;
}

static int append_to_macro(macro_node_t *macro, const char* line) {
    status_t status = STATUS_UNINITIALIZED;

    if (macro->content == NULL) {
        macro->content = (char*)malloc(strlen(line) + 1); /* Added one for null terminator */
        if (macro->content == NULL) {
            print_sys_error("Couldn't allocate memory for macro's content\n");
            status = STATUS_FAILURE_MEMORY_ALLOCATION;
            goto lb_cleanup;
        }
        strcpy(macro->content, line);
    }
    else {
        /* Reallocating memory to add content to macro */
        char* new_content = (char*)realloc(macro->content, strlen(macro->content) + strlen(line) + 1);
        if (new_content == NULL) {
            print_sys_error("Couldn't reallocate memory for macro's content\n");
            status = STATUS_FAILURE_MEMORY_ALLOCATION;
            goto lb_cleanup;
        }
        macro->content = new_content;
        strncat(macro->content, line, strlen(line));
    }
    status = STATUS_SUCCESS;
    
lb_cleanup:
return (int)status;
}

static void free_macro_table(macro_node_t *head) {
    macro_node_t *temp = NULL;
    while (head != NULL) {
        temp = head;
        head = head->next;
        if (temp->content != NULL) {
            free(temp->content);
        }
        free(temp);
    }
}

static macro_node_t* find_macro(macro_node_t *head, const char* name) {
    macro_node_t *curr = head;
    while (curr != NULL) {
        if (strcmp(curr->name, name) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL; /* Didn't find name of macro in the macros list */
}