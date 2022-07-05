//
// Created on 23/05/2022.
//

#include <stdlib.h>
#include "parser/parse_context.h"

// Parse Context
static void parser_ctx_pos_push(ParseContext * ctx)
{
    ctx->pos_stack.push(&ctx->pos_stack, ctx->pos);
}

static void parser_ctx_pos_pop(ParseContext * ctx)
{
    ctx->pos = ctx->pos_stack.pop(&ctx->pos_stack);
}

ParseContext create_parse_ctx(Token * tokens, size_t num_tokens) {
    ParseContext ctx = {
            .tokens = tokens,
            .old_token_types = malloc(num_tokens * sizeof(enum TokenType)),
            .num_tokens = num_tokens,
            .pos = 0,
            .pos_stack = create_stack(),
            .volatile_parser_results = create_stack(),
            .context_flags = 0,
            .cst = (CSTNode) {
                    .children = NULL,
                    .num_children = 0,
                    .type = NONE_PARSER,
                    .token = NULL,
            },
            .last_leaf = NULL,
            .pos_push = parser_ctx_pos_push,
            .pos_pop = parser_ctx_pos_pop,
    };
    for (size_t i = 0; i < num_tokens; i++)
        ctx.old_token_types[i] = tokens[i].type;
    ctx.pos_stack.push(&ctx.pos_stack, 0);
    return ctx;
}

// Utils for Parse Context
void append_cst_to_children(CSTNode * parent, CSTNode * child)
{
    if (parent->children == NULL || parent->num_children == 0) {
        parent->children = malloc(sizeof(CSTNode *));
        parent->num_children = 0;
    } else {
        parent->children = realloc(parent->children, (parent->num_children + 1) * sizeof(CSTNode *));
    }
    parent->children[parent->num_children++] = child;
}

void parser_commit_single_token(void * void_ctx, Parser * p, void * void_parent, void * void_child, int pos0)
{
    ParseContext * ctx = void_ctx;
    CSTNode * parent = void_parent;
    CSTNode * child = void_child;
    child->token = &ctx->tokens[pos0];
    child->children = NULL;
    child->num_children = 0;
    (void)p; // TODO: check out this child/parent type situation
    // child->type = p->type;
    append_cst_to_children(parent, child);
}

// Free function
void free_parser_ctx(ParseContext ctx) {
    free(ctx.old_token_types);
    free_stack(ctx.pos_stack);
    free_stack(ctx.volatile_parser_results);
}
