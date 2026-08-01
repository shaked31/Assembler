#include "../include/file_generator.h"
#include "../include/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int create_ob_file(const char* filename, int IC, int DC, machine_word_t *code_image, unsigned char *data_image);
static int create_ent_file(const char* filename, symbol_node_t *sym_head);
static int create_ext_file(const char* filename, ext_node_t *ext_head);

int generate_files(const char* filename, machine_word_t *code_image, unsigned char *data_image,
                        symbol_node_t **sym_head, ext_node_t **ext_head, int IC, int DC) {

    status_t status = STATUS_UNINITIALIZED;
    if ((status = create_ob_file(filename, IC, DC, code_image, data_image))) {
        return status;
    }

    if ((status = create_ent_file(filename, *sym_head))) {
        return status;
    }

    if ((status = create_ext_file(filename, *ext_head))) {
        return status;
    }

    status = STATUS_SUCCESS;
    return status;
}

static int create_ob_file(const char* filename, int IC, int DC, machine_word_t *code_image, unsigned char *data_image) {
    status_t status = STATUS_UNINITIALIZED;
    FILE *ob_fptr = NULL;
    int i = 0;
    int addr = IC_START_ADDR;
    int instruction_bytes = IC - IC_START_ADDR;

    ob_fptr = open_file_with_extension(filename, "ob", "w", &status);
    if (ob_fptr == NULL) {
        goto lb_cleanup;
    }

    fprintf(ob_fptr, "\t%d %d\n", instruction_bytes, DC);
    for (i = 0 ; i < instruction_bytes / INSTRUCTION_SIZE_BYTES ; i++) {
        fprintf(ob_fptr, "%04d %02X %02X %02X %02X\n", addr,
                code_image[i].raw_word & 0xFF,
                (code_image[i].raw_word >> 8) & 0xFF,
                (code_image[i].raw_word >> 16) & 0xFF,
                (code_image[i].raw_word >> 24) & 0xFF);
        addr += INSTRUCTION_SIZE_BYTES;
    }

    for (i = 0 ; i < DC ; i++) {
        fprintf(ob_fptr, "%04d %02X\n", addr, data_image[i]);
        addr++;
    }

    status = STATUS_SUCCESS;

lb_cleanup:
CLOSE_FILE(ob_fptr);
return (int)status;
}

static int create_ent_file(const char* filename, symbol_node_t *sym_head) {
    status_t status = STATUS_UNINITIALIZED;
    symbol_node_t *curr = sym_head;
    int has_entry = 0;
    FILE *ent_fptr = NULL;

    while (curr != NULL) {
        if (curr->is_entry == 1) {
            has_entry = 1;
            break;
        }
        curr = curr->next;
    }
    if (has_entry == 0) {
        return STATUS_SUCCESS;
    }


    ent_fptr = open_file_with_extension(filename, "ent", "w", &status);
    if (ent_fptr == NULL) {
        goto lb_cleanup;
    }

    curr = sym_head;
    while (curr != NULL) {
        if (curr->is_entry == 1) {
            fprintf(ent_fptr, "%s %04d\n", curr->name, curr->address);
        }
        curr = curr->next;
    }

    status = STATUS_SUCCESS;

lb_cleanup:
CLOSE_FILE(ent_fptr);
return (int)status;
}

static int create_ext_file(const char* filename, ext_node_t *ext_head) {
    status_t status = STATUS_UNINITIALIZED;
    ext_node_t *curr = ext_head;
    FILE *ext_fptr = NULL;

    if (ext_head == NULL) {
        return STATUS_SUCCESS;
    }

    ext_fptr = open_file_with_extension(filename, "ext", "w", &status);
    if (ext_fptr == NULL) {
        goto lb_cleanup;
    }

    while (curr != NULL) {
        fprintf(ext_fptr, "%s %04d\n", curr->name, curr->address);
        curr = curr->next;
    }

    status = STATUS_SUCCESS;

lb_cleanup:
CLOSE_FILE(ext_fptr);
return (int)status;
}