//
// Created by nicolas on 22. 6. 25.
//

#include "lexer/shell_grammar/rules.h"

#define RULE_NAME(number) [RULE_##number] = "RULE_##number"

const char * const GRAMMAR_RULE_STRING[] = {
        RULE_NAME(0),
        RULE_NAME(1),
        RULE_NAME(2),
        RULE_NAME(3),
        RULE_NAME(4),
        RULE_NAME(5),
        RULE_NAME(6a),
        RULE_NAME(6b),
        RULE_NAME(7a),
        RULE_NAME(7b),
        RULE_NAME(8),
        RULE_NAME(9),
};
