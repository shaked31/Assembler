#ifndef GLOBALS_H
#define GLOBALS_H

#define MAX_LINE_LEN (80)
#define MAX_LABEL_LEN (31)

/* Initial value for the in Instruction Counter */
#define IC_START_ADDR (100)

/* Initial value for the in Data Counter */
#define DC_START_ADDR (0)

#define NUM_OF_REGISTERS (32)

/* Size of an instruction in bytes*/
#define INSTRUCTION_SIZE_BYTES (4)

#define MAX_MEMORY_SIZE (8192)

/* for .as\0 and .am\0 */
#define FILE_EXTENTION_SIZE (4)

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


typedef enum {
    SYM_CODE,
    SYM_DATA,
    SYM_EXTERNAL
} symbol_type_t;

typedef struct symbol_node_s {
    char name[MAX_LABEL_LEN + 1]; /* For null terminator */
    int address;
    symbol_type_t type;
    unsigned char is_entry;
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
    STATUS_FAILURE_BINARY_USAGE,
    STATUS_FAILURE_MEMORY_ALLOCATION,
    STATUS_FAILURE_FILE_MGMT,
    STATUS_FAILURE_NOTHING_TO_PARSE,
    STATUS_FAILURE_LABEL_TOO_LONG,
    STATUS_FAILURE_DUPLICATE_LABEL_DEF,
    STATUS_FAILURE_DUPLICATE_EXTERNAL_SYM,
    STATUS_FAILURE_INVALID_STR_IN_ASCIZ,
    STATUS_FAILURE_UNKNOWN_OPERATION,
    STATUS_FAILURE_UNDEFINED_LABEL,
    STATUS_FAILURE_MISSING_OPERANDS

} status_t;

#endif
