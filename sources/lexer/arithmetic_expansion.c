//
// Created by Nicolas Reyland on 5/26/22.
//

#include "lexer/arithmetic_expansion.h"
#include "lexer/command_substitution.h"
#include "lexer/parameter_expansion.h"
#include "lexer/candidates.h"
#include "misc/output/output.h"

/*
 * Due to quote-removal, this could modify 'str'
 */
size_t arithmetic_expansion_end(char * str, size_t str_len) {
    size_t index = 0,
           str_len_mo = str_len - 1; // str_len minus one
    int level = 0;
    while (index < str_len)
    {
        if (is_quote(str[index])) { // " | ' | \
            // check for self-quoting (forward)
            if (str[index] == '\\' && index != str_len_mo && is_quote(str[index + 1])) {
                str[index] = str[index + 1];
                // remove the quote
                memcpy(str + index + 1, str + index + 2, str_len - index - 2);
                str[str_len--] = 0;
                index++;
                continue;
            }
            // remove the quote
            memcpy(str + index, str + index + 1, str_len - index - 1);
            str[--str_len] = 0;
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
            CANDIDATE_COMMAND_SUBSTITUTION_BRANCH // `sub` | $(sub)
            else CANDIDATE_PARAMETER_EXPANSION_BRANCH // $sub | ${sub}
            else
                index++;
            continue;
        }
        index++;
        if (level == 0)
            return index;
    }
    print_error(OCERR_EXIT, "arithmetic_expansion_end: Unclosed couple: \"$((\" & \"))\" in \"%s\" (%zu)\n", str, str_len);
}
