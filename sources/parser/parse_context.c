//
// Created on 23/05/2022.
//

#include <string.h>
#include "parser/parse_context.h"
#include "misc/safemem.h"

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
            .tokens_backup = reg_malloc(num_tokens * sizeof(Token)),
            .num_tokens = num_tokens,
            .flagged_tokens = reg_calloc(num_tokens, sizeof(unsigned long)),
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
    // deep copy of tokens
    memcpy(ctx.tokens_backup, tokens, num_tokens * sizeof(Token));
    for (size_t i = 0; i < num_tokens; i++) {
        ctx.tokens_backup[i].str = reg_malloc(tokens[i].str_len + 1);
        memcpy(ctx.tokens_backup[i].str, tokens[i].str, tokens[i].str_len + 1);
    }
    ctx.pos_stack.push(&ctx.pos_stack, 0);
    return ctx;
}

// Utils for Parse Context
void append_cst_to_children(CSTNode * parent, CSTNode * child)
{
    if (parent->children == NULL || parent->num_children == 0) {
        parent->children = reg_malloc(sizeof(CSTNode *));
        parent->num_children = 0;
    } else {
        parent->children = reg_realloc(parent->children, (parent->num_children + 1) * sizeof(CSTNode *));
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
    free_stack(ctx.pos_stack);
    free_stack(ctx.volatile_parser_results);
}
