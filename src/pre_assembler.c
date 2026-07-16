#include "../include/pre_assembler.h"
#include "../include/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* for .as\0 and .am\0 */
#define FILE_EXTENTION_SIZE 4

/**
 * @fn append_to_macro
 * @brief Helper function to append a line of text to a macro's content
 * 
 * This function operates differently and accordingly if the macro is new.
 * If it's already define and then it adds data to it using realloc and strncat
 * 
 * @param[in]  macro       Pointer to a MacroNode struct
 * @param[in]  line        String of the current line from the .as file
 * @return                 An integer of success / failure.
 */
static int append_to_macro(MacroNode *macro, const char* line);

/**
 * @fn free_macro_table
 * @brief This function safely frees all dynamically allocated memory in the macro table.
 * it iterates through the linked list, freeing the content strings first,
 * and then freeing the nodes themselves to prevent memory leaks.
 * @param[in]  head  Pointer to the head of the MacroNode linked list.
 */
static void free_macro_table(MacroNode *head);

/**
 * @fn find_macro
 * @brief Searches the macro linked list for a specific macro by name.
 * * @param[in] head Pointer to the head of the MacroNode linked list.
 * @param[in] name The name of the macro to search for.
 * @return         Pointer to the MacroNode if found, or NULL if not found.
 */
static MacroNode* find_macro(MacroNode *head, const char* name);

int run_pre_assembler(const char* filename) {
    FILE *as_fptr, *am_fptr;
    char *as_filename, *am_filename;
    char line_buffer[MAX_LINE_LEN];
    ParsedLine parsed_line;
    MacroNode *curr_mcro = NULL, *head_mcro = NULL, *found_mcro = NULL;
    unsigned char is_in_macro = 0;
    int ret = 0;
    int status = EXIT_FAILURE;

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
        if (is_in_macro) {
            if (strcmp(parsed_line.operation, "mcroend") == 0) {
                is_in_macro = 0;
                curr_mcro = NULL;
            }
            else {
                ret = append_to_macro(curr_mcro, line_buffer);
                if (ret == EXIT_FAILURE) {
                    goto lb_cleanup;
                }
            }
        }
        else {
            if (strcmp(parsed_line.operation, "mcro") == 0) {
                /* Found a new macro definition */
                MacroNode *new_node = (MacroNode*)malloc(sizeof(MacroNode));
                if (new_node == NULL) {
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
                    fputs(found_mcro->content, am_fptr);
                }
                else {
                    /* Just a normal assembly operation (like add) */
                    fputs(line_buffer, am_fptr);
                }
            }
        }
    }

    status = EXIT_SUCCESS;
    lb_cleanup:
        free_macro_table(head_mcro);
        fclose(as_fptr);
        fclose(am_fptr);
        free(as_filename);
        free(am_filename);
        return status;
}


static int append_to_macro(MacroNode *macro, const char* line) {
    if (macro->content == NULL) {
        macro->content = (char*)malloc(strlen(line) + 1); /* Added one for null terminator */
        if (macro->content == NULL) {
            fprintf(stderr, "Couldn't allocate memory for macro's content\n");
            return EXIT_FAILURE;
        }
        cpy(macro->content, line);
    }
    else {
        /* Reallocating memory to add content to macro */
        char* new_content = (char*)realloc(macro->content, strlen(macro->content) + strlen(line) + 1);
        if (new_content == NULL) {
            fprintf(stderr, "Couldn't reallocate memory for macro's content\n");
            return EXIT_FAILURE;
        }
        macro->content = new_content;
        strncat(macro->content, line, strlen(line));
    }
    return EXIT_SUCCESS;
}

static void free_macro_table(MacroNode *head) {
    MacroNode *temp = NULL;
    while (head != NULL) {
        temp = head;
        head = head->next;
        if (temp->content != NULL) {
            free(temp->content);
        }
        free(temp);
    }
}

static MacroNode* find_macro(MacroNode *head, const char* name) {
    MacroNode *curr = head;
    while (curr != NULL) {
        if (strncmp(curr->name, name, strlen(name)) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL; /* Didn't find name of macro in the macros list */
}