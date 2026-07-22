#include "../include/symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int insert_symbol(SymbolNode **head, const char* name, int address,
                    unsigned int is_code, unsigned int is_data, 
                    unsigned int is_entry, unsigned int is_external) {
    SymbolNode *new_sym = NULL;
    new_sym = (SymbolNode*)malloc(sizeof(SymbolNode));
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

SymbolNode* find_symbol(SymbolNode *head, const char* name) {
    SymbolNode *curr = head;
    while (curr != NULL) {
        if(strcmp(curr->name, name) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

void free_symbol_table(SymbolNode *head) {
    SymbolNode *temp = NULL;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}