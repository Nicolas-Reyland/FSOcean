//
// Created by Nicolas Reyland on 5/10/22.
//

#ifndef OCEAN_TOKENS_H
#define OCEAN_TOKENS_H

#include <stddef.h>
#include <stdbool.h>
#include "states.h"

typedef struct {
    char * str;
    size_t str_len;
    STATE state;
    int char_index;
    bool eof_or_empty;
} Token;

Token * tokenize(const char* content, size_t* num_tokens);

#endif // OCEAN_TOKENS_H
