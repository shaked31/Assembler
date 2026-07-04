#ifndef PARSER_H
#define PARSER_H

#include "globals.h"

typedef struct {
    char label[MAX_LABLE_LEN + 1];
    char operation[MAX_LABLE_LEN];
    char operands[MAX_LABLE_LEN];
} ParsedLine;

int parse_line(char* rawline, ParsedLine* parsed_res);

int is_empty_or_comment(char* line);

#endif