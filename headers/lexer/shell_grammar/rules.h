//
// Created by nicolas on 22. 6. 25.
//

#ifndef OCEAN_RULES_H
#define OCEAN_RULES_H

#include "combinators/combinator.h"

typedef enum GRAMMAR_RULE GRAMMAR_RULE;
enum GRAMMAR_RULE {
    RULE_1,
    RULE_2,
    RULE_3,
    RULE_4,
    RULE_5,
    RULE_6, // there are 6a & 6b, but bundled together
    RULE_7a,
    RULE_7b,
    RULE_8,
    RULE_9,
};

extern const char * const GRAMMAR_RULE_STRING[];
extern const cmb_exec_function GRAMMAR_RULE_DECORATOR[];

bool GRAMMAR_RULE_2_decorator(void * void_ctx, Combinator * cmb);
bool GRAMMAR_RULE_3_decorator(void * void_ctx, Combinator * cmb);
bool GRAMMAR_RULE_4_decorator(void * void_ctx, Combinator * cmb);
bool GRAMMAR_RULE_5_decorator(void * void_ctx, Combinator * cmb);
bool GRAMMAR_RULE_6_decorator(void * void_ctx, Combinator * cmb);
bool GRAMMAR_RULE_7a_decorator(void * void_ctx, Combinator * cmb);
bool GRAMMAR_RULE_7b_decorator(void * void_ctx, Combinator * cmb);
bool GRAMMAR_RULE_8_decorator(void * void_ctx, Combinator * cmb);
bool GRAMMAR_RULE_9_decorator(void * void_ctx, Combinator * cmb);

#endif //OCEAN_RULES_H
