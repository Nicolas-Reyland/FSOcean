//
// Created by Nicolas Reyland on 5/26/22.
//

#include <stdio.h>
#include <stdlib.h>
#include "lexer/parameter_expansion.h"
#include "lexer/command_substitution.h"
#include "string_utils/string_utils.h"

size_t parameter_expansion_end(const char * str, size_t str_len) {
    size_t index = 0;
    int level = 0;
    while (index < str_len)
    {
        // straight up skip the escaped characters if asked to
        if (str[index] == '\\') {
            index += 2;
            continue;
        }
        // hard-coded levels
        if (str[index] == '{') {
            level++;
        } else if (str[index] == '}') {
            level--;
        } else if (str[index] == '"') {
            index += find_corresponding_char(str + index, str_len - index, 0, '"', true);
        } else if (str[index] == '`' || (index != str_len - 1 && str[index] == '$' && str[index] == '(')) {
            index += command_substitution_end(str + index, str_len - index, str[index - 1]);
        }
        index++;
        if (level == 0)
            return index;
    }
    fprintf(stderr, "parameter_expansion_end: Unclosed couple: '{' & '}' in \"%s\" (%zu)\n", str, str_len);
    exit(1);
}

size_t parameter_expansion_no_brackets_end(const char * str, size_t str_len) {
    size_t num_chars = 0;
    while (num_chars < str_len)
    {
        if (str[num_chars] == '\\') {
            num_chars += 2;
            continue;
        }
        if (is_blank_char(str[num_chars]) || is_special_char(str[num_chars], -1)) {
            return num_chars;
        }
        num_chars++;
    }
    return num_chars;
}
