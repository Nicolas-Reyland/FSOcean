//
// Created on 12/05/2022.
//

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "combinators/combinator.h"
#include "parser/cst.h"
#include "parser/parse_context.h"

// Parser
Parser parser_create(
        parser_exec_function exec,
        parser_exec_function exec_f,
        parser_commit_function commit
)
{
    return (Parser) {
            .type = COMBINATOR_NONE_TYPE,
            .sub_parsers = NULL,
            .num_sub_parsers = 0,
            .decorator = NULL,
            .exec_f = exec_f,
            .exec = exec,
            .commit = commit,
            .parser_generator = NULL,
    };
}

static inline void append_parser_single_child(Parser * parent, Parser * child) {
    assert(parent->num_sub_parsers == 0);
    assert(parent->sub_parsers == NULL);
    parent->num_sub_parsers = 1;
    parent->sub_parsers = malloc(sizeof(struct Parser));
    memcpy(parent->sub_parsers, child, sizeof(struct Parser));
}

static inline Parser retrieve_parser_single_child(Parser * parent) {
    assert(parent->num_sub_parsers == 1);
    assert(parent->sub_parsers != NULL);
    return parent->sub_parsers[0];
}

bool execute_parser(void * ctx, Parser * p)
{
    if (p->decorator == NULL)
        return p->exec_f(ctx, p);
    else
        return p->decorator(ctx, p);
}

// Forward Ref
static bool forward_ref_exec_f(void * void_ctx, Parser * generator)
{
    // Generate origin parser
    Parser origin = generator->parser_generator();
    // Keep type on local stack
    ParserType generator_type = generator->type;
    // Overwrite self with origin
    memcpy(generator, &origin, sizeof(Parser));
    // Check for special type
    if (generator_type != COMBINATOR_GENERATOR_TYPE)
        generator->type = generator_type;
    // Execute final parser of self
    return execute_parser(void_ctx, generator);
}

Parser parser_forward_ref(parser_exec_function parser_exec, struct Parser (*parser_generator)(void))
{
    Parser p = parser_create(parser_exec, forward_ref_exec_f, NULL);
    p.type = COMBINATOR_GENERATOR_TYPE;
    p.parser_generator = parser_generator;
    return p;
}

// Inverted
static bool parser_inverted_parse_f(void * void_ctx, Parser * p)
{
    // Cast ctx
    ParseContext * ctx = void_ctx;
    return !p->sub_parsers[0].exec(ctx, &p->sub_parsers[0]);
}

Parser parser_inverted(parser_exec_function parser_exec, Parser p)
{
    Parser inverted = typed_parser(
            parser_create(parser_exec, parser_inverted_parse_f, parser_commit_single_token),
            COMBINATOR_INVERTED_TYPE);
    inverted.num_sub_parsers = 1;
    inverted.sub_parsers = malloc(sizeof(Parser));
    inverted.sub_parsers[0] = p;
    return inverted;
}

// Sequence
static bool parser_sequence_parse_f(void * void_ctx, Parser * p)
{
    ParseContext * ctx = void_ctx;
    CSTNode * parent = NULL, * seq_child = NULL;
    //
    parent = ctx->last_leaf;
    seq_child = malloc(sizeof(CSTNode));
    seq_child->children = NULL;
    seq_child->num_children = 0;
    seq_child->token = NULL;
    seq_child->type = COMBINATOR_SEQUENCE_UNIT_TYPE;
    ctx->last_leaf = seq_child;

    int pos0 = ctx->pos;
    for (size_t i = 0; i < p->num_sub_parsers; i++) {
        Parser sub_parser = p->sub_parsers[i];
        if (!sub_parser.exec(ctx, &sub_parser)) {
            ctx->pos = pos0;
            // free sequence node
            free_cst_node(seq_child);
            // re-set the last leaf
            ctx->last_leaf = parent;
            return false;
        }
    }

    // manually commit the sequence child to parent node
    append_cst_to_children(parent, seq_child);
    // re-set the last leaf
    ctx->last_leaf = parent;
    return true;
}

static void parser_sequence_commit(void * void_ctx, Parser * p, void * void_parent, void * void_child, int pos0)
{
    (void)void_ctx;
    (void)pos0;
    (void)p;
    CSTNode * parent = void_parent;
    CSTNode * child = void_child;
    child->token = NULL;
    // child->type = p->type;
    append_cst_to_children(parent, child);
}

Parser parser_sequence(parser_exec_function parser_exec, unsigned int count, ...)
{
    Parser * parsers = calloc(count, sizeof(Parser));

    va_list args;
    va_start(args, count);

    for (unsigned int i = 0; i < count; i++)
    {
        parsers[i] = va_arg(args, Parser);
    }

    va_end(args);

    // Create Parser and return it
    Parser parser = parser_create(parser_exec, parser_sequence_parse_f, parser_sequence_commit);
    parser.sub_parsers = parsers;
    parser.num_sub_parsers = count;
    parser.type = COMBINATOR_SEQUENCE_TYPE;

    return parser;
}

// Repetition
static bool parser_repetition_exec_f(void * ctx, Parser * p)
{
    Parser sub_parser = retrieve_parser_single_child(p);
    int count = 0;
    bool success = sub_parser.exec_f(ctx, &sub_parser);
    for (; success; ) {
        count++;
        success = sub_parser.exec_f(ctx, &sub_parser);
    }
    return true;
}

static void parser_repetition_commit(void * void_ctx, Parser * p, void * void_parent, void * void_child, int pos0)
{
    (void)void_ctx;
    (void)p;
    (void)pos0;
    CSTNode * parent = void_parent;
    CSTNode * child = void_child;
    child->token = NULL;
    append_cst_to_children(parent, child);
}

Parser parser_repetition(parser_exec_function parser_exec, Parser p) {
    Parser parser = parser_create(parser_exec, parser_repetition_exec_f, parser_repetition_commit);
    append_parser_single_child(&parser, &p);
    parser.type = COMBINATOR_REPETITION_TYPE;
    return parser;
}

// Optional
static bool parser_optional_parse_f(void * void_ctx, Parser * p)
{
    ParseContext * ctx = void_ctx;
    Parser sub_parser = retrieve_parser_single_child(p);
    bool real_success = sub_parser.exec(void_ctx, &sub_parser);
    ctx->volatile_parser_results.push(&ctx->volatile_parser_results, real_success);
    return true;
}

static void parser_optional_commit(void * void_ctx, Parser * p, void * void_parent, void * void_child, int pos0)
{
    // first commit next token
    parser_commit_single_token(void_ctx, p, void_parent, void_child, pos0);
    // set token to NULL if needed
    ParseContext * ctx = void_ctx;
    bool success = ctx->volatile_parser_results.pop(&ctx->volatile_parser_results);
    if (!success) {
        CSTNode * child = void_child;
        child->token = NULL;
    }
}

Parser parser_optional(parser_exec_function parser_exec, Parser opt_parser) {
    Parser parser = parser_create(parser_exec, parser_optional_parse_f, parser_optional_commit);
    parser.exec_f = parser_optional_parse_f;
    append_parser_single_child(&parser, &opt_parser);
    parser.type = COMBINATOR_OPTIONAL_TYPE;
    return parser;
}

// Choice
static bool parser_choice_parse_f(void * void_ctx, Parser * p)
{
    ParseContext * ctx = void_ctx;
    for (size_t i = 0; i < p->num_sub_parsers; i++) {
        Parser sub_parser = p->sub_parsers[i];
        if (sub_parser.exec(ctx, &sub_parser)) {
            ctx->volatile_parser_results.push(&ctx->volatile_parser_results, 1);
            return true;
        }
    }
    ctx->volatile_parser_results.push(&ctx->volatile_parser_results, 0);
    return false;
}

static void parser_choice_commit(void * void_ctx, Parser * p, void * void_parent, void * void_child, int pos0)
{
    (void)p;
    (void)pos0;
    // Cast
    ParseContext * ctx = void_ctx;
    CSTNode * parent = void_parent;
    CSTNode * child = void_child;
    int success = ctx->volatile_parser_results.pop(&ctx->volatile_parser_results);
    // success should be 0 or 1
    if (success) {
        append_cst_to_children(parent, child);
    }
}

Parser parser_choice(parser_exec_function parser_exec, unsigned int count, ...)
{
    Parser * parsers = calloc(count, sizeof(Parser));

    va_list args;
    va_start(args, count);

    for (unsigned int i = 0; i < count; i++)
    {
        parsers[i] = va_arg(args, Parser);
    }

    va_end(args);

    // Create Parser and return it
    Parser parser = parser_create(parser_exec, parser_choice_parse_f, parser_choice_commit);
    parser.sub_parsers = parsers;
    parser.num_sub_parsers = count;
    parser.type = COMBINATOR_CHOICE_TYPE;

    return parser;
}

// Separated Parsers
static bool separated_parser_exec_f(void * void_ctx, Parser * p)
{
    Parser sub_parser = retrieve_parser_single_child(p);
    if (!sub_parser.exec_f(void_ctx, &sub_parser))
        return false;
    // Cast ctx
    ParseContext * ctx = void_ctx;
    // Remove the sequence_unit cst node
    assert(ctx->last_leaf->num_children == 1);
    ParserType last_leaf_type = ctx->last_leaf->type;
    CSTNode * seq_unit = ctx->last_leaf->children[0];
    memcpy(ctx->last_leaf, seq_unit, sizeof(CSTNode));
    ctx->last_leaf->type = last_leaf_type;
    free(seq_unit);
    return true;
}

Parser parser_separated(parser_exec_function parser_exec, Parser p, Parser separator)
{
    Parser parser = parser_create(parser_exec, separated_parser_exec_f, parser_commit_single_token),
               separated_seq_p = parser_sequence(parser_exec, 2,
                                         p,
                                         typed_parser(
                                                 parser_repetition(
                                                         parser_exec,
                                                         parser_sequence(parser_exec,2,
                                                                 separator,
                                                                 p
                                                         )
                                                 ),
                                         COMBINATOR_SEPARATED_REPETITION_TYPE
                                         )
                                 );
    parser.type = COMBINATOR_SEPARATED_TYPE;
    append_parser_single_child(&parser, &separated_seq_p);
    return parser;
}

// Lookahead
static bool lookahead_parser_parse(void * void_ctx, Parser * p)
{
    // Cast ctx
    ParseContext * ctx = void_ctx;
    ParseContext dummy_ctx = *ctx;
    size_t last_leaf_num_children = ctx->last_leaf->num_children;
    bool result = execute_parser(&dummy_ctx, &p->sub_parsers[0]);
    size_t last_leaf_num_children_after = dummy_ctx.last_leaf->num_children;
    // free up all that was added
    if (last_leaf_num_children != dummy_ctx.last_leaf->num_children) {
        // remove the children that got added during the lookahead step
        for (size_t i = last_leaf_num_children; i < last_leaf_num_children_after; i++)
            free_cst_node(ctx->last_leaf->children[i]);
        ctx->last_leaf->num_children = last_leaf_num_children;
        ctx->last_leaf->children = realloc(ctx->last_leaf->children,  last_leaf_num_children * sizeof(CSTNode *));
    }
    return result;
}

Parser parser_lookahead(parser_exec_function parser_exec, Parser p) {
    Parser lookahead_p = parser_create(parser_exec, NULL, NULL);
    lookahead_p.num_sub_parsers = 1;
    lookahead_p.sub_parsers = malloc(sizeof(Parser));
    lookahead_p.sub_parsers[0] = p;
    // Yes, overwriting 'exec' itself
    lookahead_p.exec = lookahead_parser_parse;
    return typed_parser(
            lookahead_p,
            COMBINATOR_LOOKAHEAD_TYPE);
}

// Typed (misc)
Parser typed_parser(Parser p, int type) {
    p.type = type;
    return p;
}
