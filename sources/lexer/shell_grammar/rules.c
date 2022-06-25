//
// Created by nicolas on 22. 6. 25.
//

#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "lexer/shell_grammar/rules.h"
#include "parser/parse_context.h"
#include "impl.h"
#include "string_utils/string_utils.h"

#define RULE_STRING(number) [GRAMMAR_RULE_##number] = "GRAMMAR_RULE_##number"
#define RULE_DECORATOR(number) [GRAMMAR_RULE_##number] = GRAMMAR_RULE_##number##_decorator

const char * const GRAMMAR_RULE_STRING[] = {
        RULE_STRING(1),
        RULE_STRING(2),
        RULE_STRING(3),
        RULE_STRING(4),
        RULE_STRING(5),
        RULE_STRING(6),
        RULE_STRING(7a),
        RULE_STRING(7b),
        RULE_STRING(8),
        RULE_STRING(9),
};

const cmb_exec_function GRAMMAR_RULE_DECORATOR[] = {
        RULE_DECORATOR(1),
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

bool GRAMMAR_RULE_1_decorator(void * void_ctx, Combinator * cmb) {
    ParseContext * ctx = void_ctx;
    Token * token = &ctx->tokens[ctx->pos];
    // Check for exact reserved word
    size_t word_index = 0;
    for (; word_index < NUM_GRAMMAR_RESERVED_WORDS; word_index++) {
        if (strcmp(GRAMMAR_RESERVED_WORDS[word_index], token->str) == 0) {
            token->type = GRAMMAR_RESERVED_WORDS_TYPES[word_index];
            break;
        }
    }
    // Was not found ? WORD results
    if (word_index == NUM_GRAMMAR_RESERVED_WORDS) {
        token->type = WORD_TOKEN;
    }
    return cmb->exec_f(void_ctx, cmb);
}

bool GRAMMAR_RULE_2_decorator(void * void_ctx, Combinator * cmb) {
    bool success = cmb->exec_f(void_ctx, cmb);
    if (success) {
        //
    }
    return success;
}

bool GRAMMAR_RULE_3_decorator(void * void_ctx, Combinator * cmb) {
    bool success = cmb->exec_f(void_ctx, cmb);
    if (success) {
        //
    }
    return success;
}

bool GRAMMAR_RULE_4_decorator(void * void_ctx, Combinator * cmb) {
    bool success = cmb->exec_f(void_ctx, cmb);
    if (success) {
        //
    }
    return success;
}

bool GRAMMAR_RULE_5_decorator(void * void_ctx, Combinator * cmb) {
    bool success = cmb->exec_f(void_ctx, cmb);
    if (success) {
        //
    }
    return success;
}

bool GRAMMAR_RULE_6_decorator(void * void_ctx, Combinator * cmb) {
    bool success = cmb->exec_f(void_ctx, cmb);
    if (success) {
        //
    }
    return success;
}

bool GRAMMAR_RULE_7a_decorator(void * void_ctx, Combinator * cmb) {
    ParseContext * ctx = void_ctx;
    Token token = ctx->tokens[ctx->pos];
    size_t i = 0;
    for (; i < token.str_len && token.str[i] != '='; i++);
    if (i == token.str_len) {
        // Apply rule 1
        return GRAMMAR_RULE_1_decorator(void_ctx, cmb);
    }
    // Apply rule 7b
    return GRAMMAR_RULE_7b_decorator(void_ctx, cmb);
}

bool GRAMMAR_RULE_7b_decorator(void * void_ctx, Combinator * cmb) {
    ParseContext * ctx = void_ctx;
    Token * token = &ctx->tokens[ctx->pos];
    ssize_t equal_index = contains_unquoted_char(token->str, token->str_len, '=');
    // Apply rule 1 if no '=' or '=' is first char
    if (equal_index == -1 || equal_index == 0)
        return GRAMMAR_RULE_1_decorator(void_ctx, cmb);
    // ASSIGNMENT_WORD
    if (forms_valid_xbd_name(token->str, (size_t)index)) {
        token->type = ASSIGNMENT_WORD_TOKEN;
        return cmb->exec_f(void_ctx, cmb);
    }
    // Unspecified behaviour
    // ?
    fprintf(stderr, "Unspecified behaviour expected at %s (%d:%d)\n",
            token->str,
            token->line_index,
            token->char_index);
    exit(1);
    // return cmb->exec_f(void_ctx, cmb);
}

bool GRAMMAR_RULE_8_decorator(void * void_ctx, Combinator * cmb) {
    bool success = cmb->exec_f(void_ctx, cmb);
    if (success) {
        //
    }
    return success;
}

bool GRAMMAR_RULE_9_decorator(void * void_ctx, Combinator * cmb) {
    bool success = cmb->exec_f(void_ctx, cmb);
    if (success) {
        //
    }
    return success;
}
