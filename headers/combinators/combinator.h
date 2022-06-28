//
// Created on 12/05/2022.
//

#ifndef OCEAN_COMBINATOR_H
#define OCEAN_COMBINATOR_H

#include <stdbool.h>
#include "atom_type.h"

typedef struct Parser Parser;
struct Parser;

typedef bool (*parser_exec_function)(void *, struct Parser *);
typedef void (*parser_commit_function)(void *, struct Parser *, void *, void *, int);

struct Parser {
    ParserType type;
    struct Parser * sub_parsers;
    size_t num_sub_parsers;
    parser_exec_function decorator;
    parser_exec_function exec;
    parser_exec_function exec_f;
    parser_commit_function commit;
    // for forward-referencing
    struct Parser (*parser_generator)(void);
};

struct Parser parser_create(
        parser_exec_function exec,
        parser_exec_function exec_f,
        parser_commit_function commit
);
bool execute_parser(void * ctx, Parser * p);

Parser parser_forward_ref(parser_exec_function parser_exec, struct Parser (*parser_generator)(void));

Parser parser_inverted(parser_exec_function, Parser);
Parser parser_sequence(parser_exec_function, unsigned int, ...);
Parser parser_repetition(parser_exec_function, Parser);
Parser parser_optional(parser_exec_function, Parser);
Parser parser_choice(parser_exec_function, unsigned int, ...);
Parser parser_separated(parser_exec_function, Parser, Parser);
Parser parser_lookahead(parser_exec_function, Parser);

Parser typed_parser(Parser, int);

#endif // OCEAN_COMBINATOR_H
