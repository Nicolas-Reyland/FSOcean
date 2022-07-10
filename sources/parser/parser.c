//
// Created on 23/05/2022.
//

#include <assert.h>
#include "parser/parser.h"
#include "parser/parse_context.h"
#include "misc/safemem.h"

#ifdef OCEAN_DEBUG_MACRO
#include <stdio.h>
#endif

bool parser_parse(void * void_ctx, Parser * p)
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
        printf("%d %s : (%s) '%s'\n",
               depth,
               PARSER_TYPE_STRING(p->type),
               TOKEN_TYPE_STRING(ctx->tokens[ctx->pos].type),
               ctx->tokens[ctx->pos].str[0] == '\n' ? "\\n" : ctx->tokens[ctx->pos].str
        );
#endif

    // for the first call of exec function
    if (ctx->last_leaf == NULL)
        ctx->last_leaf = &ctx->cst;
    // setup local variables
    CSTNode * prev_leaf = ctx->last_leaf;
    CSTNode * cur_leaf = reg_malloc(sizeof(CSTNode));
    cur_leaf->type = p->type;
    cur_leaf->children = NULL;
    cur_leaf->num_children = 0;
    cur_leaf->token = NULL;
    ctx->last_leaf = cur_leaf;
    int pos0 = ctx->pos;
    ctx->pos_push(ctx);
    // exec next token
    bool success = execute_parser(ctx, p);
    assert(ctx->pos_stack.peek(&ctx->pos_stack) == pos0);
    if (success) {
        // Add CST Node(s)
        assert(p->commit != NULL);
        p->commit(ctx, p, prev_leaf, cur_leaf, pos0);
        ctx->pos_stack.pop(&ctx->pos_stack);
    } else {
        // reset token types of processed tokens
        for (size_t i = pos0; i < ctx->pos; i++) {
            if (ctx->flagged_tokens[i] & FLAGGED_TOKEN_SET) {
//                if (ctx->flagged_tokens[i] & FLAGGED_TOKEN_TYPE)
//                    ctx->tokens[i].type = ctx->tokens_backup[i].type;
                if (ctx->flagged_tokens[i] & FLAGGED_TOKEN_STRING) {
                    ctx->tokens[i].str = ctx->tokens_backup[i].str;
                    ctx->tokens[i].str_len = ctx->tokens_backup[i].str_len;
                }
                ctx->flagged_tokens[i] = 0;
            }
        }
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

