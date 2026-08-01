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
static const char *skip_whitespaces(const char *pstr) {
    while (*pstr && isspace(*pstr)) {
        pstr++;
    }
    return pstr;
}

int is_empty_or_comment(const char* line) {
    const char *pfirst_non_whitespace = skip_whitespaces(line);
    if (*pfirst_non_whitespace == ';' || *pfirst_non_whitespace == '\0' || *pfirst_non_whitespace == '\n')
        return 1;

    return 0;
}

int parse_line(const char* rawline, parsed_line_t* parsed_res) {
    const char *ptr_current = rawline;
    char *ptr_colon;
    int label_len;
    int i = 0;
    status_t status = STATUS_UNINITIALIZED;

    if (is_empty_or_comment(ptr_current)) {
        /* Nothing to parse */
        status = STATUS_FAILURE_NOTHING_TO_PARSE;
        goto lb_cleaup;
    }

    /* Checks for label */
    ptr_colon = strchr(ptr_current, ':');
    if (ptr_colon) {
        label_len = ptr_colon - ptr_current;
        if (label_len > MAX_LABEL_LEN) {
            fprintf(stderr, "Label length is too long\n");
            status = STATUS_FAILURE_LABEL_TOO_LONG;
            goto lb_cleaup;
        }
        strncpy(parsed_res->label, ptr_current, label_len);
        parsed_res->label[label_len] = '\0';
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
    while (*ptr_current && *ptr_current != '\n' && *ptr_current != '\r') {
        parsed_res->operands[i] = *ptr_current;
        i++;
        ptr_current++;
    }
    
    /* Backtrack and erase any trailing spaces at the end of the string */
    while (i > 0 && isspace((unsigned char)parsed_res->operands[i - 1])) {
        parsed_res->operands[i - 1] = '\0';
        i--;
    }
    
    status = STATUS_SUCCESS;
    
lb_cleaup:
return (int)status;
}