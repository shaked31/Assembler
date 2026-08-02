
#ifndef PARSER_H
#define PARSER_H

/**
 * @file parser.h
 * @brief Parse each line of the assembly file as a struct
 * @author Shaked Pollak, Daniela Aslan
 */

#include "globals.h"

/**
 * @brief Represents a line of the assembly file.
 */
typedef struct {
    char label[MAX_LABEL_LEN + 1];
    char operation[MAX_LABEL_LEN];
    char operands[MAX_LINE_LEN];
} parsed_line_t;

/**
 * @fn parse_line
 * @brief The actual parser function of a given line to the struct parsed_line_t
 * This function parses through a line, searches for ':' and by that pointer gets the label
 * After that, it searches for the next non-blanc char and register the whole word as the operation
 * At the end, it takes all the operands and puts it as a string to the struct
 * 
 * @param[in]  rawline       String of an entire line from the file
 * @param[in]  parsed_res    Pointer to parsed_line_t struct, the function changes its content
 * @param[in]  asm_line_num  An integer of the line count of the assembly source file
 * @return                   An integer of success / failure
 */
int parse_line(const char* rawline, parsed_line_t* parsed_res, unsigned int asm_line_num);

/**
 * @fn is_empty_or_comment
 * @brief Goes through the line and checks if its a comment or an empty line.
 * 
 * @param[in]  line  String of an entire line from the file.
 * @return           An integer of success / failure.
 */
int is_empty_or_comment(const char* line);

#endif /* PARSER_H */