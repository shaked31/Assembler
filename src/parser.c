#include "../include/parser.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Increments a pointer until non-whitespace char.
 * 
 * @param[in]  line  Pointer to a char.
 * @return           Pointer to the next non-whitespace char.
 */
static char *skip_whitespaces(char *pstr) {
    while (*pstr && isspace(*pstr)) {
        pstr++;
    }
    return pstr;
}

int is_empty_or_comment(char* line) {
    char *pfirst_non_whitespace = skip_whitespaces(line);
    if (*pfirst_non_whitespace == ';' || *pfirst_non_whitespace == '\0' || *pfirst_non_whitespace == '\n')
        return 1;

    return 0;
}

int parse_line(char* rawline, ParsedLine* parsed_res) {
    char *ptr_current = rawline;
    char *ptr_colon;
    int label_len;
    int i = 0;

    if (is_empty_or_comment(ptr_current)) {
        return 1;  /* Nothing to parse */
    }

    /* Checks for label */
    ptr_colon = strchr(ptr_current, ':');
    if (ptr_colon) {
        label_len = ptr_colon - ptr_current;
        if (label_len > MAX_LABEL_LEN) {
            fprintf(stderr, "Label length is too long\n");
            return EXIT_FAILURE;
        }
        strncpy(parsed_res->label, ptr_current, label_len);
        ptr_current = ptr_colon + 1;
    }

    ptr_current = skip_whitespaces(ptr_current);

    /* Checks for operation */
    while (*ptr_current && !isspace(*ptr_current)) {
        parsed_res->operation[i] = *ptr_current;
        i++;
        ptr_current++;
    }

    ptr_current = skip_whitespaces(ptr_current);
    i = 0;
    /* Checks for operands */
    while (*ptr_current && *ptr_current != '\n') {
        parsed_res->operands[i] = *ptr_current;
        i++;
        ptr_current++;
    }
    
    return EXIT_SUCCESS;
}