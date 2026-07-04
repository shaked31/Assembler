#ifndef GLOBALS_H
#define GLOBALS_H

#define MAX_LINE_LEN 80
#define MAX_LABLE_LEN 31

/* Initial value for the in Instruction Counter */
#define IC_START_ADDR 100

/* Initial value for the in Data Counter */
#define DC_START_ADDR 0

#define NUM_OF_REGISTERS 32

/* Size of an instruction in bytes*/
#define INSTRUCTION_SIZE_BYTES 4

#define MAX_MEMORY_SIZE 8192

typedef struct SymbolNode {
    char name[MAX_LABLE_LEN + 1]; /* For null terminator */
    int address;

    unsigned char is_code;
    unsigned char is_data;
    unsigned char is_entry;
    unsigned char is_external;
    
    struct SymbolNode *next; /* Pointer for the next symbol in the list */
} SymbolNode;

typedef struct MacroNode {
    char name[MAX_LABLE_LEN + 1];
    char* content;
    struct MacroNode* next; /* Pointer for the next macro in the list */
} MacroNode;

#endif
