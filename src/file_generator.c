#include "../include/file_generator.h"
#include "../include/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @fn create_ob_file
 * @brief Creates an ob file using the code and data images, the instruction and data counter
 * @param[in]  filename      The base filename (without extension)
 * @param[in]  code_image    Array representing the instruction memory
 * @param[in]  data_image    Array representing the data memory
 * @param[in]  IC            An integer of the current instruction counter
 * @param[in]  DC            An integer of the current data counter
 * @return                   An integer of status based on status_t enum
 */
static int create_ob_file(const char* filename, machine_word_t *code_image, unsigned char *data_image, int IC, int DC);

/**
 * @fn create_ent_file
 * @brief Creates an ent file of symbol and address
 * It goes through the symbol linked list and checks if there's an entry symbol
 * If so it adds it to the file
 * 
 * @param[in]  filename      The full name of the .am file to go through
 * @param[in]  sym_head      Pointer of the head of the symbol linked list linked list
 * @return                   An integer of status based on status_t enum
 */
static int create_ent_file(const char* filename, symbol_node_t *sym_head);

/**
 * @fn create_ext_file
 * @brief Creates an ext file of symbol and address
 * It goes through the external symbols linked list and checks if there's an entry symbol
 * If so it adds it to the file
 * 
 * @param[in]  filename      The full name of the .am file to go through
 * @param[in]  ext_head      Pointer to the head of the externals linked list
 * @return                   An integer of status based on status_t enum
 */
static int create_ext_file(const char* filename, ext_node_t *ext_head);

int generate_files(const char* filename, machine_word_t *code_image, unsigned char *data_image,
                        symbol_node_t **sym_head, ext_node_t **ext_head, int IC, int DC) {

    status_t status = STATUS_UNINITIALIZED;
    if ((status = create_ob_file(filename, code_image, data_image, IC, DC))) {
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

static int create_ob_file(const char* filename, machine_word_t *code_image, unsigned char *data_image, int IC, int DC) {
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