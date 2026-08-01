#ifndef EXT_TRACKER_H
#define EXT_TRACKER_H

#include "globals.h"

typedef struct ext_node_s {
    char name[MAX_LABEL_LEN + 1];
    int address;
    struct ext_node_s* next; /* Pointer for the next external label in the list */
} ext_node_t;

/**
 * @fn add_ext_record
 * @brief This function adds a record of an external symbol usage to the linked list
 * 
 * @param[in, out]  head            Pointer the head of the external nodes linked list
 * @param[in]       name            The name of the external symbol
 * @param[in]       address         The memory address where it was called
 * @return                          An integer of status based on status_t enum
 */
int add_ext_record(ext_node_t **head, const char* name, int address);

void free_ext_list(ext_node_t *head);

#endif