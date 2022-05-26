//
// Created by Nicolas Reyland on 5/26/22.
//

#include <stdio.h>
#include <stdlib.h>
#include "lexer/arithmetic_expansion.h"
#include "lexer/command_substitution.h"
#include "lexer/parameter_expansion.h"
#include "lexer/candidates.h"

size_t arithmetic_expansion_end(const char * str, size_t str_len) {
    size_t index = 0;
    int level = 0;
    while (index < str_len)
    {
        if (str[index] == '\\') { // \
            index += 2;
            continue;
        }
        // hard-coded levels
        if (str_is_prefix(str + index, "$((")) {
            index += 2;
            level++;
        } else if (str_is_prefix(str + index, "))")) {
            index++;
            level--;
        } else {
            CANDIDATE_DOUBLE_QUOTES_BRANCH // "sub"
            else CANDIDATE_SINGLE_QUOTES_BRANCH // 'sub'
            else CANDIDATE_COMMAND_SUBSTITUTION_BRANCH // `sub` | $(sub)
            else CANDIDATE_PARAMETER_EXPANSION_BRANCH // $sub | ${sub}
            else
                index++;
            continue;
        }
        index++;
        if (level == 0)
            return index;
    }
    fprintf(stderr, "arithmetic_expansion_end: Unclosed couple: \"$((\" & \"))\" in \"%s\" (%zu)\n", str, str_len);
    exit(1);
}
