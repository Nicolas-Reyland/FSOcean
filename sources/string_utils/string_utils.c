//
// Created by Nicolas Reyland on 5/26/22.
//

#include <stdio.h>
#include "string_utils/string_utils.h"
#include "misc/output.h"

/*
 * Returns the offset from 'str' to the corresponding char.
 * The input string should include the opening char we are trying
 * to find the 'sibling' of (see below).
 *
 * Example:
 *  Input:
 *      str = "(123(56)8)XX"
 *      open = '('
 *      close = ')'
 *  Returns:
 *      9, which points to: "(123(56)8" -> ')' <- "XX"
 *
 */
size_t find_corresponding_char(const char * str, const size_t str_len, const char open, const char close, bool backslash_escape)
{
    size_t index = 0;
    int level = 0;
    if (open == 0) {
        // for single-closing/opening chars couples (such as single-quotes, double-quotes, '`', etc)
        level = 1;
        index = 1;
    }
    while (index < str_len)
    {
        // straight up skip the escaped characters if asked to
        if (backslash_escape && str[index] == '\\') {
            index += 2;
            continue;
        }
        // First check for close (quotes, etc)
        if (str[index] == close) {
            level--;
        } else if (str[index] == open) {
            level++;
        }
        index++;
        if (level == 0)
            return index;
    }
    print_error(OCERR_EXIT, "find_corresponding_char: Unclosed couple: '%c' & '%c' in \"%s\" (%zu)\n", open, close, str, str_len);
}

bool str_is_prefix(const char * str, const char * prefix) {
    size_t index = 0;
    while (prefix[index] != 0 && str[index] != 0 && prefix[index] == str[index])
        index++;
    return prefix[index] == 0;
}

extern inline bool is_blank_char(char c);

extern inline bool is_special_char(char c, CHAR_CATEGORY category);

extern inline bool is_quote(char c);

bool str_is_only_digits(char * str)
{
    for (; *str > 0x2f && *str < 0x3a; str++);
    return *str == 0;
}

ssize_t contains_unquoted_char(char * str, size_t str_len, char c) {
    size_t index = 0;
    while (index < str_len) {
        // unquoted char found
        if (str[index] == c)
            return (ssize_t)index;
        switch (str[index]) {
            case '"': case '\'':
                index += find_corresponding_char(str + index, str_len - index, str[index], str[index], true);
                break;
            case '\\':
                index += 2;
                break;
            default:
                index++;
                break;
        }
    }
    return -1;
}

static bool is_digit(char c) {
    return '0' <= c && c <= '9';
}

static bool is_alphabetic(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

bool forms_valid_xbd_name(char * str, size_t str_len) {
    size_t index = 0;
    while (index < str_len && (
            str[index] == '_' ||
            is_alphabetic(str[index]) ||
            (index != 0 && is_digit(str[index]))
      )
    ) {
        index++;
    }
    return index == str_len;
}
