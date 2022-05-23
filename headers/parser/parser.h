//
// Created on 23/05/2022.
//

#ifndef OCEAN_PARSER_H
#define OCEAN_PARSER_H

#include "combinators/combinator.h"

bool parser_parse(void *, Combinator *);

#define PARSER_CREATE(exec_f, commit) (cmb_create(parser_parse, exec_f, commit))

#define PARSER_CMB_INVERTED(parser) (cmb_inverted(parser_parse, parser))
#define PARSER_CMB_SEPARATED(value, separator) (cmb_separated(parser_parse, (value), (separator)))
#define PARSER_CMB_SEQUENCE(...) (cmb_sequence(parser_parse, __VA_ARGS__))
#define PARSER_CMB_CHOICE(...) (cmb_choice(parser_parse, __VA_ARGS__))

#define PARSER_CMB_LOOKAHEAD(parser) (cmb_lookahead(parser_parse, parser))

#define PARSER_CMB_FORWARD_REF(generator) (cmb_forward_ref(parser_parse, generator))

#endif // OCEAN_PARSER_H
