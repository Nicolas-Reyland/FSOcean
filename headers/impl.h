//
// Created on 17/05/2022.
//

#ifndef OCEAN_IMPL_H
#define OCEAN_IMPL_H

#include "combinators/combinator.h"
#include "parser/parse_context.h"

extern const char * const GRAMMAR_RESERVED_WORDS[];

extern const size_t NUM_GRAMMAR_RESERVED_WORDS;

extern const TokenType GRAMMAR_RESERVED_WORDS_TYPES[];

Combinator program_parser();

#endif // OCEAN_IMPL_H
