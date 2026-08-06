#include "../include/ext_tracker.h"
#include "../include/error_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int add_ext_record(ext_node_t **head, const char* name, int address) {
    status_t status = STATUS_UNINITIALIZED;
    ext_node_t *new_node = NULL;
    new_node = (ext_node_t*)malloc(sizeof(*new_node));
    
    if (new_node == NULL) {
        print_sys_error("Couldn't allocate memory for .ext file extension\n");
        status = STATUS_FAILURE_MEMORY_ALLOCATION;
        goto lb_cleanup;
    }

    strcpy(new_node->name, name);
    new_node->address = address;
    new_node->next = *head;
    *head = new_node;

    status = STATUS_SUCCESS;

lb_cleanup:
return (int)status;
}

void free_ext_list(ext_node_t *head) {
    ext_node_t *temp = NULL;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}