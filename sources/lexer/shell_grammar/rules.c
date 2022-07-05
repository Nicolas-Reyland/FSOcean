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
#include "misc/impl.h"
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

const parser_exec_function GRAMMAR_RULE_DECORATOR[] = {
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

bool GRAMMAR_RULE_1_decorator(void * void_ctx, Parser * parser) {
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
    return parser->exec_f(void_ctx, parser);
}

bool GRAMMAR_RULE_2_decorator(void * void_ctx, Parser * parser) {
    // TODO: implement rule 2 (redirection)
    return parser->exec_f(void_ctx, parser);
}

bool GRAMMAR_RULE_3_decorator(void * void_ctx, Parser * parser) {
    // TODO: implement rule 3 (here-document)
    return parser->exec_f(void_ctx, parser);
}

bool GRAMMAR_RULE_4_decorator(void * void_ctx, Parser * parser) {
    ParseContext * ctx = void_ctx;
    Token * token = &ctx->tokens[ctx->pos];
    if (token->str_len == 4 && token->str != NULL && strcmp(token->str, "esac") == 0)
        token->type = ESAC_TOKEN;
    return parser->exec_f(void_ctx, parser);
}

bool GRAMMAR_RULE_5_decorator(void * void_ctx, Parser * parser) {
    ParseContext * ctx = void_ctx;
    Token * token = &ctx->tokens[ctx->pos];
    if (forms_valid_xbd_name(token->str, token->str_len))
        token->type = NAME_TOKEN;
    else
        token->type = WORD_TOKEN;
    return parser->exec_f(void_ctx, parser);
}

bool GRAMMAR_RULE_6_decorator(void * void_ctx, Parser * parser) {
    ParseContext * ctx = void_ctx;
    Token * token = &ctx->tokens[ctx->pos];
    // Impossible? case
    if (ctx->pos < 2) {
        fprintf(stderr, "rule 6: Unspecified behaviour expected at %s (%d:%d)\n",
                token->str,
                token->line_index,
                token->char_index);
        exit(1);
    }
    // Case only
    if (ctx->tokens[ctx->pos - 2].type == CASE_TOKEN) {
        if (token->str_len == 2 && token->str != NULL && strcmp(token->str, "in") == 0)
            token->type = IN_TOKEN;
        else
            token->type = WORD_TOKEN;
    } else if (ctx->tokens[ctx->pos - 2].type == FOR_TOKEN) {
        if (token->str_len == 2 && token->str != NULL && strcmp(token->str, "in") == 0)
            token->type = IF_TOKEN;
        else if (token->str_len == 2 && token->str != NULL && strcmp(token->str, "do") == 0)
            token->type = DO_TOKEN;
        else
            token->type = WORD_TOKEN;
    } else {
        token->type = WORD_TOKEN;
    }
    return parser->exec_f(void_ctx, parser);
}

static bool GRAMMAR_RULE_7_decorator(void * void_ctx, Parser * parser) {
    ParseContext * ctx = void_ctx;
    if (ctx->last_leaf->type == CMD_PREFIX_PARSER)
        return GRAMMAR_RULE_7b_decorator(void_ctx, parser);
    return GRAMMAR_RULE_7a_decorator(void_ctx, parser);
}

bool GRAMMAR_RULE_7a_decorator(void * void_ctx, Parser * parser) {
    ParseContext * ctx = void_ctx;
    Token token = ctx->tokens[ctx->pos];
    size_t i = 0;
    for (; i < token.str_len && token.str[i] != '='; i++);
    if (i == token.str_len) {
        // Apply rule 1
        return GRAMMAR_RULE_1_decorator(void_ctx, parser);
    }
    // Apply rule 7b
    return GRAMMAR_RULE_7b_decorator(void_ctx, parser);
}

bool GRAMMAR_RULE_7b_decorator(void * void_ctx, Parser * parser) {
    ParseContext * ctx = void_ctx;
    Token * token = &ctx->tokens[ctx->pos];
    ssize_t equal_index = contains_unquoted_char(token->str, token->str_len, '=');
    // Apply rule 1 if no '=' or '=' is first char
    if (equal_index == -1 || equal_index == 0)
        return GRAMMAR_RULE_1_decorator(void_ctx, parser);
    // ASSIGNMENT_WORD
    if (forms_valid_xbd_name(token->str, (size_t)equal_index)) {
        token->type = ASSIGNMENT_WORD_TOKEN;
        return parser->exec_f(void_ctx, parser);
    }
    // Unspecified behaviour
    // ?
    fprintf(stderr, "rule 7b: Unspecified behaviour expected at %s (%d:%d)\n",
            token->str,
            token->line_index,
            token->char_index);
    exit(1);
    // return parser->exec_f(void_ctx, parser);
}

bool GRAMMAR_RULE_8_decorator(void * void_ctx, Parser * parser) {
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
    // Was not found ? then WORD results
    if (word_index == NUM_GRAMMAR_RESERVED_WORDS) {
        if (forms_valid_xbd_name(token->str, token->str_len))
            token->type = NAME_TOKEN;
        else
            return GRAMMAR_RULE_7_decorator(void_ctx, parser);
    }
    return parser->exec_f(void_ctx, parser);
}

bool GRAMMAR_RULE_9_decorator(void * void_ctx, Parser * parser) {
    // TODO: implement rule 9 (body of function)
    return parser->exec_f(void_ctx, parser);
}
