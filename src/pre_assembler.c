#include "../include/globals.h"
#include "../include/pre_assembler.h"
#include "../include/parser.h"
#include "../include/symbol_table.h"

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
    status_t status = STATUS_UNINITIALIZED;
    FILE *as_fptr = NULL, *am_fptr = NULL;
    char *as_filename = NULL, *am_filename = NULL;
    char line_buffer[MAX_LINE_LEN];
    parsed_line_t parsed_line = { 0 };
    macro_node_t *curr_mcro = NULL, *head_mcro = NULL, *found_mcro = NULL, *new_node = NULL;
    unsigned char is_in_macro = 0;
    int ret = 0;

    as_filename = (char*)malloc(strlen(filename) + FILE_EXTENTION_SIZE);
    am_filename = (char*)malloc(strlen(filename) + FILE_EXTENTION_SIZE);

    if (as_filename == NULL) {
        fprintf(stderr, "Couldn't allocate memory for .as file extention\n");
        status = STATUS_FAILURE_MEMORY_ALLOCATION;
        goto lb_cleanup;
    }
    if (am_filename == NULL) {
        fprintf(stderr, "Couldn't allocate memory for .am file extention\n");
        status = STATUS_FAILURE_MEMORY_ALLOCATION;
        goto lb_cleanup;
    }

    sprintf(as_filename, "%s.as", filename);
    sprintf(am_filename, "%s.am", filename);

    as_fptr = fopen(as_filename, "r");
    if (as_fptr == NULL) {
        fprintf(stderr, "Couldn't open file %s in read mode\n", as_filename);
        status = STATUS_FAILURE_FILE_MGMT;
        goto lb_cleanup;
    }

    am_fptr = fopen(am_filename, "w");
    if (am_fptr == NULL) {
        fprintf(stderr, "Couldn't open file %s in write mode\n", am_filename);
        status = STATUS_FAILURE_FILE_MGMT;
        goto lb_cleanup;
    }

    while (fgets(line_buffer, sizeof(line_buffer), as_fptr) != NULL) {
        memset(&parsed_line, 0, sizeof(parsed_line));
        
        if (parse_line(line_buffer, &parsed_line) == 1) {
            continue;
        }

        if (is_in_macro) {
            if (strcmp(parsed_line.operation, "mcroend") == 0) {
                is_in_macro = 0;
                curr_mcro = NULL;
            }
            else {
                ret = append_to_macro(curr_mcro, line_buffer);
                if (ret != (int)STATUS_SUCCESS) {
                    status = (status_t)ret;
                    goto lb_cleanup;
                }
            }
        }

        else {
            if (strcmp(parsed_line.operation, "mcro") == 0) {
                /* Found a new macro definition */
                new_node = (macro_node_t*)malloc(sizeof(macro_node_t));
                if (new_node == NULL) {
                    fprintf(stderr, "Couldn't allocate memory for a new macro node\n");
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
    }

    status = STATUS_SUCCESS;

lb_cleanup:
FREE_VAR(as_filename);
FREE_VAR(am_filename);
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
            fprintf(stderr, "Couldn't allocate memory for macro's content\n");
            status = STATUS_FAILURE_MEMORY_ALLOCATION;
            goto lb_cleanup;
        }
        strcpy(macro->content, line);
    }
    else {
        /* Reallocating memory to add content to macro */
        char* new_content = (char*)realloc(macro->content, strlen(macro->content) + strlen(line) + 1);
        if (new_content == NULL) {
            fprintf(stderr, "Couldn't reallocate memory for macro's content\n");
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