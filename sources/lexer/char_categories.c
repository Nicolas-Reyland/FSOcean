//
// Created by Nicolas Reyland on 5/10/22.
//

#include <stdio.h>
#include <stdlib.h>
#include "lexer/char_categories.h"

CHAR_CATEGORY char_category(const char c) {
    switch (c) {
        case ' ': case '\t':
            return CHAR_BLANK;
        case '&':
            return CHAR_AMPERSAND;
        case '|':
            return CHAR_PIPE;
        case ';':
            return CHAR_SEMICOLON;
        case '\'':
            return CHAR_SINGLE_QUOTE;
        case '"':
            return CHAR_DOUBLE_QUOTE;
        case '>':
            return CHAR_RIGHT_ANGLE_BRACKET;
        case '<':
            return CHAR_LEFT_ANGLE_BRACKET;
        case '`': case '\n': case '=': case ')': case '(':
            return CHAR_SINGLE_CHAR;
        case '\\':
            return CHAR_ESCAPE;
        case EOF: case 0x0:
            return CHAR_EOF;
        default:
            return CHAR_GENERAL;
    }
}

char * CHAR_CATEGORY_STRING(CHAR_CATEGORY cat) {
    switch (cat) {
        case CHAR_GENERAL:
            return "CHAR_GENERAL";
        case CHAR_BLANK:
            return "CHAR_BLANK";
        case CHAR_AMPERSAND:
            return "CHAR_AMPERSAND";
        case CHAR_PIPE:
            return "CHAR_PIPE";
        case CHAR_SINGLE_QUOTE:
            return "CHAR_SINGLE_QUOTE";
        case CHAR_DOUBLE_QUOTE:
            return "CHAR_DOUBLE_QUOTE";
        case CHAR_RIGHT_ANGLE_BRACKET:
            return "CHAR_RIGHT_ANGLE_BRACKET";
        case CHAR_LEFT_ANGLE_BRACKET:
            return "CHAR_LEFT_ANGLE_BRACKET";
        case CHAR_SINGLE_CHAR:
            return "CHAR_SINGLE_CHAR";
        case CHAR_ESCAPE:
            return "CHAR_ESCAPE";
        case CHAR_EOF:
            return "CHAR_EOF";
        default:
            fprintf(stderr, "Unknown char category: %d\n", cat);
            exit(1);
    }
}
