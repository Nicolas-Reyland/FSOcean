//
// Created by Nicolas Reyland on 5/26/22.
//

#include <stdio.h>
#include "lexer/command_substitution.h"
#include "lexer/arithmetic_expansion.h"
#include "lexer/parameter_expansion.h"
#include "lexer/candidates.h"

size_t command_substitution_end(char * str, size_t str_len, char mode)
{
    size_t index = mode == '`' ? 1 : 2;
    int level = 1;
    while (index < str_len)
    {
        if (str[index] == '\\') { // \
            index += 2;
            continue;
        }
        // hard-coded level (only down-leveling)
        if ((mode == '`' && str[index] == '`') || (mode == '$' && str[index] == ')'))
            level--;
        else {
            CANDIDATE_DOUBLE_QUOTES_BRANCH // "sub"
            else CANDIDATE_SINGLE_QUOTES_BRANCH // 'sub'
            else CANDIDATE_ARITHMETIC_EXPANSION_BRANCH // $((sub))
            else CANDIDATE_PARAMETER_EXPANSION_BRANCH // $sub | ${sub}
            else
                index++;
            continue;
        }
        index++;
        if (level == 0)
            return index;
    }
    fprintf(stderr, "command_substitution_end: Unclosed couple: \"$(\" & \")\" in \"%s\" (%zu)\n", str, str_len);
    exit(1);
}
