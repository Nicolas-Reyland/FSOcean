//
// Created by nicolas on 22. 6. 25.
//

#include <stddef.h>
#include "lexer/shell_grammar/rules.h"

#define RULE_NAME(number) [RULE_##number] = "RULE_##number"
#define RULE_DECORATOR(number) [RULE_##number] = GRAMMAR_RULE_##number##_decorator

const char * const GRAMMAR_RULE_STRING[] = {
        RULE_NAME(1),
        RULE_NAME(2),
        RULE_NAME(3),
        RULE_NAME(4),
        RULE_NAME(5),
        RULE_NAME(6),
        RULE_NAME(7a),
        RULE_NAME(7b),
        RULE_NAME(8),
        RULE_NAME(9),
};

const cmb_exec_function GRAMMAR_RULE_DECORATOR[] = {
        [RULE_1] = NULL,
        RULE_DECORATOR(2),
        RULE_DECORATOR(3),
        RULE_DECORATOR(4),
        RULE_DECORATOR(5),
        RULE_DECORATOR(6),
        RULE_DECORATOR(7a),
        RULE_DECORATOR(7b),
        RULE_DECORATOR(8),
        RULE_DECORATOR(9),
};

bool GRAMMAR_RULE_2_decorator(void * void_ctx, Combinator * cmb) {
    return 0;
}

bool GRAMMAR_RULE_3_decorator(void * void_ctx, Combinator * cmb) {
    return 0;
}

bool GRAMMAR_RULE_4_decorator(void * void_ctx, Combinator * cmb) {
    return 0;
}

bool GRAMMAR_RULE_5_decorator(void * void_ctx, Combinator * cmb) {
    return 0;
}

bool GRAMMAR_RULE_6_decorator(void * void_ctx, Combinator * cmb) {
    return 0;
}

bool GRAMMAR_RULE_7a_decorator(void * void_ctx, Combinator * cmb) {
    return 0;
}

bool GRAMMAR_RULE_7b_decorator(void * void_ctx, Combinator * cmb) {
    return 0;
}

bool GRAMMAR_RULE_8_decorator(void * void_ctx, Combinator * cmb) {
    return 0;
}

bool GRAMMAR_RULE_9_decorator(void * void_ctx, Combinator * cmb) {
    return 0;
}
