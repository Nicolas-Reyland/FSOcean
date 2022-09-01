//
// Created by nicolas on 22. 6. 25.
//

#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <assert.h>
#include <stdlib.h>
#include "lexer/shell_grammar/rules.h"
#include "parser/parse_context.h"
#include "misc/impl.h"
#include "string_utils/string_utils.h"
#include "eval/expansion/filename_exp.h"
#include "misc/safemem.h"
#include "misc/output/output.h"

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
    if (ctx->pos == ctx->num_tokens)
        return false;
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
    if (word_index == NUM_GRAMMAR_RESERVED_WORDS && (
            token->type == TOKEN_TOKEN ||
            token->type == NAME_TOKEN
        )
    ) {
        token->type = WORD_TOKEN;
    }
    return parser->exec_f(void_ctx, parser);
}

bool GRAMMAR_RULE_2_decorator(void * void_ctx, Parser * parser) {
    // TODO: implement rule 2 (redirection)
    ParseContext * ctx = void_ctx;
    if (ctx->pos == ctx->num_tokens)
        return false;
    int pos0 = ctx->pos;
    assert(pos0 > 0); // at least one preceding token
    bool success = parser->exec_f(void_ctx, parser);
    if (success) {
        // Only one token got consumed (filename)
        assert(ctx->pos - pos0 == 1);
        // Preceding token
        Token io_op_token = ctx->tokens[pos0 - 1];
        // Check if token is a redirection operator
        if (io_op_token.type == OPERATOR_TOKEN)
            assert(io_op_token.str_len == 1 &&
                (io_op_token.str[0] == '>' || io_op_token.str[0] == '<')
            );
        else
            assert(
                    io_op_token.type == LESSAND_TOKEN ||
                    io_op_token.type == GREATAND_TOKEN ||
                    io_op_token.type == DGREAT_TOKEN ||
                    io_op_token.type == LESSGREAT_TOKEN ||
                    io_op_token.type == CLOBBER_TOKEN
            );
        // check for pathname expansion dis-allowance
        if (ctx->context_flags & PARSE_CTX_FUNCTION_BODY_FLAG)
            return success;
        // expand word as a filepath
        Token * filename_token = &ctx->tokens[pos0];
        const char * filename = filename_token->str;
        char ** file_list = NULL;
        ssize_t num_files = expand_filename(filename, filename_token->str_len, &file_list);
        if (num_files != 1) {
            print_error(OCERR_EXIT, "rule 2: Expansion of '%s' did not result in a single word, but %ld words\n", filename, num_files);
        }
        // TODO: redo all this code once pathname expansion is properly implemented
        ctx->flagged_tokens[pos0] |= FLAGGED_TOKEN_SET_STRING;
        // reg_free(filename_token->str);
        filename_token->str_len = strlen(file_list[0]);
        filename_token->str = file_list[0];
        reg_free(file_list);
    }
    return success;
}

bool GRAMMAR_RULE_3_decorator(void * void_ctx, Parser * parser) {
    // TODO: [here-document] implement rule 3
    return parser->exec_f(void_ctx, parser);
}

bool GRAMMAR_RULE_4_decorator(void * void_ctx, Parser * parser) {
    ParseContext * ctx = void_ctx;
    if (ctx->pos == ctx->num_tokens)
        return false;
    Token * token = &ctx->tokens[ctx->pos];
    if (token->str_len == 4 && token->str != NULL && strcmp(token->str, "esac") == 0)
        token->type = ESAC_TOKEN;
    return parser->exec_f(void_ctx, parser);
}

bool GRAMMAR_RULE_5_decorator(void * void_ctx, Parser * parser) {
    ParseContext * ctx = void_ctx;
    if (ctx->pos == ctx->num_tokens)
        return false;
    Token * token = &ctx->tokens[ctx->pos];
    if (forms_valid_xbd_name(token->str, token->str_len))
        token->type = NAME_TOKEN;
    else
        token->type = WORD_TOKEN;
    return parser->exec_f(void_ctx, parser);
}

bool GRAMMAR_RULE_6_decorator(void * void_ctx, Parser * parser) {
    ParseContext * ctx = void_ctx;
    if (ctx->pos == ctx->num_tokens)
        return false;
    Token * token = &ctx->tokens[ctx->pos];
    // Impossible? case
    if (ctx->pos < 2) {
        print_error(OCERR_EXIT, "rule 6: Unspecified behaviour expected at %s (%d:%d)\n",
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
    if (ctx->pos == ctx->num_tokens)
        return false;
    if (ctx->last_leaf->type == CMD_PREFIX_PARSER)
        return GRAMMAR_RULE_7b_decorator(void_ctx, parser);
    return GRAMMAR_RULE_7a_decorator(void_ctx, parser);
}

bool GRAMMAR_RULE_7a_decorator(void * void_ctx, Parser * parser) {
    ParseContext * ctx = void_ctx;
    if (ctx->pos == ctx->num_tokens)
        return false;
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
    if (ctx->pos == ctx->num_tokens)
        return false;
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
    print_error(OCERR_EXIT, "rule 7b: Unspecified behaviour expected at %s (%d:%d)\n",
            token->str,
            token->line_index,
            token->char_index);
    exit(1);
    // return parser->exec_f(void_ctx, parser);
}

bool GRAMMAR_RULE_8_decorator(void * void_ctx, Parser * parser) {
    ParseContext * ctx = void_ctx;
    if (ctx->pos == ctx->num_tokens)
        return false;
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
    // TODO: implement rule 9 (body of function) : the rest of the rule is yet to implement (not in this function)
    ParseContext * ctx = void_ctx;
    if (ctx->pos == ctx->num_tokens)
        return false;
    int context_flags0 =  ctx->context_flags;
    ctx->context_flags |= PARSE_CTX_FUNCTION_BODY_FLAG;
    bool success = parser->exec_f(void_ctx, parser);
    ctx->context_flags = context_flags0;
    return success;
}
