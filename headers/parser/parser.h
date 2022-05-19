//
// Created on 12/05/2022.
//

#ifndef OCEAN_PARSER_H
#define OCEAN_PARSER_H

#include <stdbool.h>
#include "lexer/token.h"
#include "stack.h"
#include "cst.h"

typedef struct ParseContext {
    Token * tokens;
    size_t num_tokens;
    int pos;
    struct Stack pos_stack; // indices
    struct Stack volatile_parser_results; // choices etc.
    CSTNode cst;
    CSTNode * last_leaf;
    void (*pos_push)(struct ParseContext *);
    void (*pos_pop)(struct ParseContext *);
} ParseContext;

typedef struct Parser {
    ConcreteNodeType type;
    struct Parser * sub_parsers;
    size_t num_sub_parsers;
    bool (*decorator)(ParseContext *, struct Parser *);
    bool (*parse_f)(ParseContext *, struct Parser *);
    bool (*parse)(ParseContext *, struct Parser *);
    void (*commit)(ParseContext *, struct Parser *, CSTNode *, CSTNode *, int);
    // for forward-referencing
    struct Parser (*parser_generator)(void);
} Parser;

ParseContext create_parse_ctx(Token *, size_t);
Parser create_parser(bool (*parse_f)(ParseContext *, struct Parser *), void (*commit)(ParseContext *, struct Parser *, CSTNode *, CSTNode *, int));

Parser parser_sequence(unsigned int, ...);
Parser parser_repetition(Parser);
Parser parser_optional(Parser);
Parser parser_choice(unsigned int, ...);

Parser typed_parser(Parser, ConcreteNodeType);

void append_cst_to_children(CSTNode * parent, CSTNode * child);
void parser_commit_single_token(ParseContext *, Parser *, CSTNode * parent, CSTNode * child, int);

#endif // OCEAN_PARSER_H
