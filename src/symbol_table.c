#include "../include/symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int insert_symbol(symbol_node_t **head, const char* name, int address,
                    unsigned int is_code, unsigned int is_data, 
                    unsigned int is_entry, unsigned int is_external) {
    symbol_node_t *new_sym = NULL;
    new_sym = (symbol_node_t*)malloc(sizeof(symbol_node_t));
    if (new_sym == NULL) {
        fprintf(stderr, "Couldn't allocate memory to symbol node\n");
        return EXIT_FAILURE;
    }

    strcpy(new_sym->name, name);
    new_sym->address = address;
    new_sym->is_code = is_code;
    new_sym->is_data = is_data;
    new_sym->is_entry = is_entry;
    new_sym->is_external = is_external;

    new_sym->next = *head;
    *head = new_sym;
    
    return EXIT_SUCCESS;
}

symbol_node_t* find_symbol(symbol_node_t *head, const char* name) {
    symbol_node_t *curr = head;
    while (curr != NULL) {
        if(strcmp(curr->name, name) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

void free_symbol_table(symbol_node_t *head) {
    symbol_node_t *temp = NULL;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}