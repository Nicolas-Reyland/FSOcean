//
// Created by Nicolas Reyland on 5/26/22.
//

#include <stddef.h>
#include "lexer/xbd_operators.h"

static const char * XBD_CONTROL_OPERATORS[] = {
        "&",
        "&&",
        "(",
        ")",
        ";",
        ";;",
        "\n",
        "|",
        "||",
};

static const char * XBD_REDIRECTION_OPERATORS[] = {
        "<",
        ">",
        ">|",
        "<<",
        ">>",
        "<&",
        ">&",
        "<<-",
        "<>",
};

/*
 * Can c be a part of 'operator', when appended 'token_str', which is a prefix of 'operator' ?
 * If 'token_str' is not a prefix of 'operator', false is returned.
 *
 */
static bool operator_can_be_formed(const char * operator, const char * token_str, char c)
{
    // Is token_str a prefix of operator ?
    int index = 0;
    while (operator[index] != 0 && operator[index] == token_str[index]) {
        index++;
    }
    // Is (token_str + c) still a prefix of operator ?
    // If it is not a prefix in the first place, returns false.
    /*
     * Explanation of the two conditions :
     *  - token_str[index] == 0
     *      The token_str should be a prefix of operator, so the previous while
     *      loop should have completely 'consumed' it.
     *  - operator[index] == c
     *      The next character in the operator is equal to c, meaning the token
     *      can be extended with c and will remain a prefix of the operator.
     */
    return token_str[index] == 0 && operator[index] == c;
}

/*
 * Can the character c be part of the token that is being formed as an operator token ?
 * Meaning: if the character c is appended at the end of the token, is it still a valid
 * operator ?
 *
 * Note: If the token is not an operator token, returns false.
 */
bool can_form_operator(char * token_str, size_t token_str_len, char c, CHAR_CATEGORY category)
{
    (void)category;
    // terminate the token string first
    token_str[token_str_len] = 0;
    const size_t num_control_operators = sizeof(XBD_CONTROL_OPERATORS) / sizeof(XBD_CONTROL_OPERATORS[0]),
                 num_redirection_operators = sizeof(XBD_REDIRECTION_OPERATORS) / sizeof(XBD_REDIRECTION_OPERATORS[0]);
    for (size_t i = 0; i < num_control_operators; i++)
        if (operator_can_be_formed(XBD_CONTROL_OPERATORS[i], token_str, c))
            return true;
    for (size_t i = 0; i < num_redirection_operators; i++)
        if (operator_can_be_formed(XBD_REDIRECTION_OPERATORS[i], token_str, c))
            return true;
    return false;
}

/*
 * Can the character c be the start of a new operator token ?
 */
extern inline bool is_operator_start(char c, CHAR_CATEGORY category);
