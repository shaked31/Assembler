#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "globals.h"

/**
 * @fn insert_symbol
 * @brief Creates a new symbol and inserts it at the head of the symbol table.
 * 
 * @param[in,out]  head         Pointer to the head of the symbols linked list.
 * @param[in]      name         The name of the label/symbol.
 * @param[in]      address      The memory address (IC or DC) of the symbol.
 * @param[in]      type         Enum of types indicating the type of the symbol.
 * @param[in]      is_entry     Flag indicating if it's an entry point.
 * @return                      EXIT_SUCCESS (0) on success, EXIT_FAILURE (1) if allocation fails.
 */
int insert_symbol(symbol_node_t **head, const char* name, int address, symbol_type_t type, unsigned char is_entry);


/**
 * @fn find_symbol
 * @brief Searches the symbol table for a specific symbol by its name.
 * 
 * @param[in]  head   Pointer to the head of the symbols linked list.
 * @param[in]  name   The name of the symbol to search for.
 * @return            Pointer to the symbols if found, or NULL if not found.
 */
symbol_node_t* find_symbol(symbol_node_t *head, const char* name);

/**
 * @fn free_symbol_table
 * @brief Safely frees all dynamically allocated memory in the symbol table.
 * 
 * @param[in]  head  Pointer to the head of the SymbolNode linked list.
 */
void free_symbol_table(symbol_node_t *head);

#endif