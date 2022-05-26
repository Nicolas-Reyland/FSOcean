//
// Created by Nicolas Reyland on 5/27/22.
//

#include <assert.h>
#include "lexer/primal_token_context.h"

static AtomType operator_token_precise_type(Token token);

void lexical_conventions_rules(Token * tokens, size_t num_tokens) {
    for (size_t i = 0; i < num_tokens; i++) {
        Token token = tokens[i];
        // Rule 1
        if (token.type == OPERATOR_TOKEN) {
            /* If the token is an operator, the token identifier for that operator shall result. */
            token.type = operator_token_precise_type(token);
        }

        // Rule 2
        /* If the string consists solely of digits and
         * the delimiter character is one of '<' or '>',
         * the token identifier IO_NUMBER shall be returned. */

        // Rule 3
        /* Otherwise, the token identifier TOKEN results. */
    }
}

#define REPEAT_CHAR_OPERATOR(c, token_type) case c: { \
        if (token.str_len == 2 && token.str[1] == (c)) { \
            return token_type; \
        } \
    } break;

#define CONSECUTIVE_CHAR_OPERATOR(c1, c2, token_type) case c1: { \
        if (token.str_len == 2 && token.str[1] == (c2)) { \
            return token_type; \
        } \
    }

static AtomType operator_token_precise_type(Token token) {
    assert(token.str != NULL && token.str_len > 1);
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
