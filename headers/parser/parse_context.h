//
// Created on 23/05/2022.
//

#ifndef OCEAN_PARSE_CONTEXT_H
#define OCEAN_PARSE_CONTEXT_H

typedef struct ParseContext ParseContext;
struct ParseContext;

#include "lexer/token.h"
#include "cst.h"
#include "combinators/combinator.h"

struct ParseContext {
    Token * tokens;
    size_t num_tokens;
    int pos;
    struct Stack pos_stack; // indices
    struct Stack volatile_parser_results; // choices etc.
    CSTNode cst;
    CSTNode * last_leaf;
    void (*pos_push)(struct ParseContext *);
    void (*pos_pop)(struct ParseContext *);
};

ParseContext create_parse_ctx(Token *, size_t);

void append_cst_to_children(CSTNode * parent, CSTNode * child);
void parser_commit_single_token(void *, Combinator *, void * parent, void * child, int);

#endif // OCEAN_PARSE_CONTEXT_H
