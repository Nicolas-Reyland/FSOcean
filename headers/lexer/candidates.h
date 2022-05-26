//
// Created by Nicolas Reyland on 5/26/22.
//

#ifndef OCEAN_CANDIDATES_H
#define OCEAN_CANDIDATES_H

#include <stdbool.h>
#include <string.h>
#include "string_utils/string_utils.h"

#define CANDIDATE_DOUBLE_QUOTES_BRANCH \
if (str[index] == '"') { \
    index += find_corresponding_char(str + index, str_len - index, 0, '"', true); \
}

#define CANDIDATE_SINGLE_QUOTES_BRANCH \
if (str[index] == '\'') { \
    index += find_corresponding_char(str + index, str_len - index, 0, '\'', true); \
}

#define CANDIDATE_ARITHMETIC_EXPANSION_BRANCH \
if (str_is_prefix(str + index, "$((")) { \
    index += arithmetic_expansion_end(str + index, str_len - index); \
}

#define CANDIDATE_COMMAND_SUBSTITUTION_BRANCH \
if (str[index] == '`' || str_is_prefix(str + index, "$(")) { \
    index += command_substitution_end(str + index, str_len - index, str[index]); \
}

#define CANDIDATE_PARAMETER_EXPANSION_BRANCH \
if (str[index] == '$' && str_is_prefix(str + index, "$((")) { \
    if (str_is_prefix(str + index, "${")) { \
        index += parameter_expansion_no_brackets_end(str + index, str_len - index); \
    } else { \
        index += parameter_expansion_end(str + index, str_len - index); \
    } \
}

#endif // OCEAN_CANDIDATES_H
