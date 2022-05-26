//
// Created by Nicolas Reyland on 5/27/22.
//

#include <assert.h>
#include "lexer/primal_token_context.h"
#include "string_utils/string_utils.h"

static AtomType operator_token_precise_type(Token token);

void lexical_conventions_rules(Token * tokens, size_t num_tokens) {
    for (size_t i = 0; i < num_tokens; i++) {
        // Rule 1
        if (tokens[i].type == OPERATOR_TOKEN) {
            /* If the token is an operator, the token identifier for that operator shall result. */
            tokens[i].type = operator_token_precise_type(tokens[i]);
            continue;
        }

        // Rule 2
        if (str_is_only_digits(tokens[i].str) &&
            i + 1 != num_tokens &&
            tokens[i + 1].type == OPERATOR_TOKEN &&
            tokens[i + 1].str_len >= 1 &&
            tokens[i].char_index + tokens[i].str_len == tokens[i + 1].char_index &&
            (tokens[i + 1].str[0] == '<' || tokens[i + 1].str[0] == '>')) {
            /* If the string consists solely of digits and
            * the delimiter character is one of '<' or '>',
            * the token identifier IO_NUMBER shall be returned. */
            tokens[i].type = IO_NUMBER_TOKEN;
            continue;
        }

        // Rule 3
        /* Otherwise, the token identifier TOKEN results. */
        tokens[i].type = TOKEN_TOKEN;
    }
}

#define REPEAT_CHAR_OPERATOR(c, token_type) case c: { \
        if (token.str_len == 2 && token.str[1] == (c)) { \
            return token_type; \
        } \
    } break;

static AtomType operator_token_precise_type(Token token) {
    assert(token.str != NULL && token.str_len != 0);
    switch (token.str[0]) {
        case '\n':
            return NEWLINE_TOKEN;
        REPEAT_CHAR_OPERATOR('&', AND_IF_TOKEN)
        REPEAT_CHAR_OPERATOR('|', OR_IF_TOKEN)
        REPEAT_CHAR_OPERATOR(';', DSEMI_TOKEN)
        case '<':
            if (token.str_len == 2) {
                if (token.str[1] == '<')
                    return DLESS_TOKEN;
                if (token.str[1] == '&')
                    return LESSAND_TOKEN;
                if (token.str[1] == '>')
                    return LESSGREAT_TOKEN;
            } else if (token.str_len == 3 && token.str[1] == '<' && token.str[2] == '-')
                return DLESSDASH_TOKEN;
            break;
        case '>':
            if (token.str_len == 2) {
                if (token.str[1] == '>')
                    return DGREAT_TOKEN;
                if (token.str[1] == '&')
                    return GREATAND_TOKEN;
                if (token.str[1] == '|')
                    return CLOBBER_TOKEN;
            }
            break;
        default:
            break;
    }
    return token.type;
}
