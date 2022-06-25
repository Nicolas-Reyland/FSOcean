//
// Created on 23/05/2022.
//

#include <stdlib.h>
#include <assert.h>
#include "parser/parser.h"
#include "parser/parse_context.h"

#ifdef OCEAN_DEBUG_MACRO
#include <stdio.h>
#endif

bool parser_parse(void * void_ctx, Combinator * p)
{
    // Cast ctx
    ParseContext * ctx = void_ctx;
    // Check for token availability
    if (ctx->pos >= ctx->num_tokens)
        return false;

#ifdef OCEAN_DEBUG_MACRO
    static int depth = 0;
    depth++;

    for (int i = 0; i < depth; i++) putchar('\t');
    printf("%d : %s '%s'\n",
           depth,
           PARSER_TYPE_STRING(p->type),
           ctx->tokens[ctx->pos].str[0] == '\n' ? "\\n" : ctx->tokens[ctx->pos].str
    );
#endif

    // for the first call of exec function
    if (ctx->last_leaf == NULL)
        ctx->last_leaf = &ctx->cst;
    // setup local variables
    CSTNode * prev_leaf = ctx->last_leaf;
    CSTNode * cur_leaf = malloc(sizeof(CSTNode));
    cur_leaf->type = CST_NONE; // p->type;
    cur_leaf->children = NULL;
    cur_leaf->num_children = 0;
    cur_leaf->token = NULL;
    ctx->last_leaf = cur_leaf;
    int pos0 = ctx->pos;
    ctx->pos_push(ctx);
    // exec next token
    bool success = execute_cmb(ctx, p);
    if (success) {
        // Add CST Node(s)
        assert(p->commit != NULL);
        p->commit(ctx, p, prev_leaf, cur_leaf, pos0);
    } else {
        // reset token types of processed tokens
        for (size_t i = pos0; i < ctx->pos; i++)
            ctx->tokens[i].type = ctx->old_token_types[i];
        // reset token pos
        ctx->pos_pop(ctx);
        // free commits
        free_cst_node(cur_leaf);
    }
    ctx->last_leaf = prev_leaf;
#ifdef OCEAN_DEBUG_MACRO
    depth--;
#endif
    return success;
}

