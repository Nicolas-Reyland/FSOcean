//
// Created by Nicolas Reyland on 5/10/22.
//

#ifndef OCEAN_TOKEN_H
#define OCEAN_TOKEN_H

#include <stddef.h>
#include <stdbool.h>
#include "atom_type.h"

typedef struct {
    char * str;
    size_t str_len;
    AtomType type;
    int line_index;
    int char_index;
} Token;

Token * tokenize(const char * content, size_t * num_tokens);

#endif // OCEAN_TOKEN_H
