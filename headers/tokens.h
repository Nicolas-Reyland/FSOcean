//
// Created by Nicolas Reyland on 5/10/22.
//

#ifndef FSOCEAN_TOKENS_H
#define FSOCEAN_TOKENS_H

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    char str[32];
    size_t str_len;
    bool eof_or_empty;
} Token;

Token * tokenize(const char* content, size_t* num_tokens);

#endif // FSOCEAN_TOKENS_H
