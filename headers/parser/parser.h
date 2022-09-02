//
// Created on 23/05/2022.
//

#ifndef OCEAN_PARSER_H
#define OCEAN_PARSER_H

#include "combinators/combinator.h"

bool parser_parse(void *, Parser *);

#define PARSER_CREATE(exec_f, commit) (parser_create(parser_parse, exec_f, commit))

#define PARSER_INVERTED(parser) (parser_inverted(parser_parse, parser))
#define PARSER_OPTIONAL(parser) (parser_optional(parser_parse, parser))
#define PARSER_SEPARATED(value, separator) (parser_separated(parser_parse, (value), (separator)))
#define PARSER_SEQUENCE(...) (parser_sequence(parser_parse, __VA_ARGS__))
#define PARSER_CHOICE(...) (parser_choice(parser_parse, __VA_ARGS__))
#define PARSER_REPETITION(parser) (parser_repetition(parser_parse, parser))

#define PARSER_LOOKAHEAD(parser) (parser_lookahead(parser_parse, parser))

#define PARSER_FORWARD_REF(generator) (parser_forward_ref(parser_parse, generator))

#define PARSER_ONE_OR_MORE(parser) \
    (PARSER_SEQUENCE(2, parser, PARSER_REPETITION(parser)))

#define PARSER_OPTIONAL_SEQUENCE(...) PARSER_OPTIONAL(PARSER_SEQUENCE(__VA_ARGS__))

#endif // OCEAN_PARSER_H
