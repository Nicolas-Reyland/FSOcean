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

#define PARSE_CTX_FUNCTION_BODY_FLAG 0x001

#define FLAGGED_TOKEN_SET    0b0001
#define FLAGGED_TOKEN_STRING 0b0010
#define FLAGGED_TOKEN_TYPE   0b0100
#define FLAGGED_TOKEN_SET_STRING (FLAGGED_TOKEN_SET | FLAGGED_TOKEN_STRING)
#define FLAGGED_TOKEN_SET_TYPE   (FLAGGED_TOKEN_SET | FLAGGED_TOKEN_TYPE)

struct ParseContext {
    Token * tokens;
    Token * tokens_backup;
    size_t num_tokens;
    unsigned long * flagged_tokens;
    int pos;
    struct Stack pos_stack; // indices
    struct Stack volatile_parser_results; // choices etc.
    int context_flags;
    CSTNode cst;
    CSTNode * last_leaf;
    void (*pos_push)(struct ParseContext *);
    void (*pos_pop)(struct ParseContext *);
};

ParseContext create_parse_ctx(Token *, size_t);

void append_cst_to_children(CSTNode * parent, CSTNode * child);
void parser_commit_single_token(void *, Parser *, void * parent, void * child, int);

// Free function
void free_parser_ctx(ParseContext ctx);

#endif // OCEAN_PARSE_CONTEXT_H
