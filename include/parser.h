/**
 * @file parser.h
 * @brief Parse each line of the assembly file as a struct
 * @author Shaked Pollak, Daniela Aslan
 */

#ifndef PARSER_H
#define PARSER_H

#include "globals.h"

/**
 * @brief Represents a line of the assembly file.
 */
typedef struct {
    char label[MAX_LABEL_LEN + 1];
    char operation[MAX_LABEL_LEN];
    char operands[MAX_LABEL_LEN];
} ParsedLine;

/**
 * @fn parse_line
 * @brief The actual parser function of a given line to the struct ParsedLine.
 * 
 * This function parses through a line, searches for ':' and by that pointer gets the label.
 * After that, it searches for the next non-blanc char and register the whole word as the operation.
 * At the end, it takes all the operands and puts it as a string to the struct.
 * 
 * @param[in]  rawline     String of an entire line from the file.
 * @param[in]  parsed_res  Pointer to ParsedLine struct, the function changes its content.
 * @return                 An integer of success / failure.
 */
int parse_line(char* rawline, ParsedLine* parsed_res);

/**
 * @fn         is_empty_or_comment
 * @brief            Goes through the line and checks if its a comment or an empty line.
 * 
 * @param[in]  line  String of an entire line from the file.
 * @return           An integer of success / failure.
 */
int is_empty_or_comment(char* line);

#endif