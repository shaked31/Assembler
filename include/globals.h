#ifndef GLOBALS_H
#define GLOBALS_H

#define MAX_LINE_LEN 80
#define MAX_LABEL_LEN 31

/* Initial value for the in Instruction Counter */
#define IC_START_ADDR 100

/* Initial value for the in Data Counter */
#define DC_START_ADDR 0

#define NUM_OF_REGISTERS 32

/* Size of an instruction in bytes*/
#define INSTRUCTION_SIZE_BYTES 4

#define MAX_MEMORY_SIZE 8192

#define FREE_VAR(var)\
   do {\
        if (var != NULL) { \
            free(var);\
        }\
    } while(0)


#define CLOSE_FILE(file)\
    do {\
        if (file != NULL) { \
            fclose(file);\
        }\
    } while(0)

typedef struct symbol_node_s {
    char name[MAX_LABEL_LEN + 1]; /* For null terminator */
    int address;
    unsigned char is_code;
    unsigned char is_data;
    unsigned char is_entry;
    unsigned char is_external;
    struct symbol_node_s *next; /* Pointer for the next symbol in the list */
} symbol_node_t;

typedef struct macro_node_s {
    char name[MAX_LABEL_LEN + 1];
    char* content;
    struct macro_node_s* next; /* Pointer for the next macro in the list */
} macro_node_t;

typedef enum status_e {
    STATUS_UNINITIALIZED = -1,
    STATUS_SUCCESS,
    STATUS_FAILURE_MEMORY_ALLOCATION,
    STATUS_FAILURE_FILE_MGMT,
    STATUS_FAILURE_NOTHING_TO_PARSE,
    STATUS_FAILURE_LABEL_TOO_LONG,
    STATUS_FAILURE_DUPLICATE_LABEL_DEF,
    STATUS_FAILURE_UNKNOWN_OPERATION
} status_t;

#endif
