//
// Created on 11/05/2022.
//

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "eval/string_eval.h"
#include "lexer/char_categories.h"
#include "string_utils/string_utils.h"
#include "misc/safemem.h"

static char escape_char(char c);

static size_t seek_sub_expr_len(const char * str, size_t str_len);

static size_t eval_sub_expr(const char * c, char * buffer, size_t len);

/*
 * Returns the escaped version of c. For example, if c == 'n', a new-line char
 * is returned.
 *
 * If c is not an escapable char (such as 'c' or ' '), zero (0) is returned.
 *
 */
static char escape_char(const char c) {
    switch (c) {
        case 'a':
            return 0x7;
        case 'b':
            return 0x8;
        case 'e':
            return 0x1b;
        case 'f':
            return 0xc;
        case 'n':
            return 0xa;
        case 'r':
            return 0xd;
        case 't':
            return 0x9;
        case 'v':
            return 0xb;
        default:
            return 0;
    }
}

/*
 * Basically, in string evaluation, there are exactly two special characters:
 * The dollar '$' character for sub-expressions and such, and the backslash '\' character.
 * Note that special characters such as new-lines and tabs have already been evaluated
 * during the tokenization process. Escaping of e.g. double-quotes and backslashes
 * have not been processed
 *
 * Args:
 *  - str: pointer to input string, with quotes at the beginning and end. will be updated (see Note)
 *  - str_len: length of the input string 'str' (counting the quotes)
 * Returns:
 *  length of new string
 *
 * Note:
 *  New string does not have any quotes
 *
 */
size_t eval_double_quoted_string(char ** str, size_t str_len, bool free_str)
{
    char result[256];
    size_t index = 0,
        result_len = 0,
        quoted_len = str_len - 2;
    for (char * c = *str + 1; index < quoted_len && *c != 0; c++, index++) {
        // escaping through backslash
        if (*c == '\\') {
            if (index == quoted_len - 1) {
                fprintf(stderr, "Bad lexing. String cannot end with non-escaped backslash\n");
                exit(1);
            }
            char next_char = *(c+1);
            switch (next_char) {
                case '$': case '\\': case '"':
                    break;
                default:
                    result[result_len] = '\\';
                    break;
            }
            result[result_len++] = next_char;
            // skip 'next_char'
            index++;
            c++;
        }
        // sub-expressions
        else if (*c == '$') {
            size_t sub_expr_len = seek_sub_expr_len(c, str_len - index - 1);
            if (sub_expr_len == 0) {
                result[result_len++] = *c;
                continue;
            }
            char sub_eval[2048];
            size_t sub_eval_len = eval_sub_expr(c + 1, sub_eval, sub_expr_len);
            // copy evaluation result to final string
            // TODO: should add verification that sub-eval result size does not exceed stack-array size
            memcpy(result + result_len, sub_eval, sub_eval_len);
            // advance indices and counters
            result_len += sub_eval_len;
            index += sub_expr_len;
            c += sub_expr_len;
        }
        // anything else, really
        else
            result[result_len++] = *c;
    }

    // terminate result string
    result[result_len++] = 0x0;
    // free old string, allocate new with updated value (not reallocating to avoid unnecessary copy)
    if (free_str)
        reg_free(*str);
    *str = reg_malloc(result_len);
    memcpy(*str, result, result_len);
    // return new string length
    return result_len;
}

// returns an offset
static size_t seek_sub_expr_len(const char * str, size_t str_len)
{
    if (str_len == 0)
        return 0;
    char c = *(str + 1);
    // sub-shells
    if (c == '(') {
        return find_corresponding_char(str, str_len, '(', ')', false);
    } else if (c == '{') {
        return find_corresponding_char(str, str_len, '{', '}', false);
    }
    // variable type
    size_t index = 1; // skip the '$'
    CHAR_CATEGORY char_cat = char_category(c);
    while (index < str_len && char_cat == CHAR_GENERAL) {
        c = str[index++];
        char_cat = char_category(c);
    }
    return index - 2; // 2: skip dollar & one too much
}

// 'c' should not be prefixed with the dollar char from the shell command
static size_t eval_sub_expr(const char * c, char * buffer, size_t len) {
    if (*c == '(') {
        // sub-shell
        strcpy(buffer, "command");
        return 7;
    } else if (*c == '{') {
        // other stuff (idk, have to look it up)
        memcpy(buffer, "braces", 5);
        return 5;
    } else {
        // variable
        char var_name[len + 1];
        // char * var_name = reg_malloc((len + 1));
        memcpy(var_name, c, len);
        var_name[len] = 0x0;
        char * value = getenv(var_name);
        if (value == NULL)
            return 0;
        size_t value_len = strlen(value);
        strcpy(buffer, value);
        return value_len;
    }
}
