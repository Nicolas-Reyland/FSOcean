//
// Created by Nicolas Reyland on 5/26/22.
//

#ifndef OCEAN_XBD_OPERATORS_H
#define OCEAN_XBD_OPERATORS_H

#include <stdbool.h>
#include "char_categories.h"

bool can_form_operator(char * token_str, size_t token_str_len, char c, CHAR_CATEGORY category);

inline bool is_operator_start(char c, CHAR_CATEGORY category)
{
    (void)category;
    return \
        // XBD Control Operators
            c == '&'  ||
            c == '('  ||
            c == ')'  ||
            c == ';'  ||
            c == '\n' ||
            c == '|'  ||
            // XBD Redirection Operators
            c == '<'  ||
            c == '>'  ;
}

#endif // OCEAN_XBD_OPERATORS_H
