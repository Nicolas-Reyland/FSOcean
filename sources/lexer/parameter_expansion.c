//
// Created by Nicolas Reyland on 5/26/22.
//

#include "lexer/parameter_expansion.h"
#include "lexer/command_substitution.h"
#include "lexer/arithmetic_expansion.h"
#include "lexer/candidates.h"
#include "misc/output.h"

size_t parameter_expansion_end(char * str, size_t str_len) {
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
        if (str[index] == '{')
            level++;
        else if (str[index] == '}')
            level--;
        else {
            CANDIDATE_DOUBLE_QUOTES_BRANCH // "sub"
            else CANDIDATE_SINGLE_QUOTES_BRANCH // 'sub'
            else CANDIDATE_COMMAND_SUBSTITUTION_BRANCH // `sub` | $(sub)
            else CANDIDATE_ARITHMETIC_EXPANSION_BRANCH // $((sub))
            else
                index++;
            continue;
        }
        index++;
        if (level == 0)
            return index;
    }
    print_error(OCERR_EXIT, "parameter_expansion_end: Unclosed couple: '{' & '}' in \"%s\" (%zu)\n", str, str_len);
}

size_t parameter_expansion_no_brackets_end(char * str, size_t str_len) {
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
