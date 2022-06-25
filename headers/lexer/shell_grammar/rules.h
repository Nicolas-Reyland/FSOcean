//
// Created by nicolas on 22. 6. 25.
//

#ifndef OCEAN_RULES_H
#define OCEAN_RULES_H

#include "combinators/combinator.h"

typedef enum GRAMMAR_RULE GRAMMAR_RULE;
enum GRAMMAR_RULE {
    GRAMMAR_RULE_1,
    GRAMMAR_RULE_2,
    GRAMMAR_RULE_3,
    GRAMMAR_RULE_4,
    GRAMMAR_RULE_5,
    GRAMMAR_RULE_6, // there are 6a & 6b, but bundled together
    GRAMMAR_RULE_7a,
    GRAMMAR_RULE_7b,
    GRAMMAR_RULE_8,
    GRAMMAR_RULE_9,
};

extern const char * const GRAMMAR_RULE_STRING[];
extern const cmb_exec_function GRAMMAR_RULE_DECORATOR[];

bool GRAMMAR_RULE_1_decorator(void * void_ctx, Combinator * cmb);
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
