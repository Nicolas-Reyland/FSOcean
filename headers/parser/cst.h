//
// Created on 16/05/2022.
//

#ifndef OCEAN_CST_H
#define OCEAN_CST_H

#include <stddef.h>
#include "lexer/token.h"
#include "combinators/stack.h"
#include "combinators/combinator.h"

typedef enum {
    CST_NONE = COMBINATOR_NONE_TYPE,
    // Shell Command Language grammar rules names
    CST_SHELL_INSTRUCTION,          // -> AST_COMMAND_LIST

    CST_COMMAND,                    // -> AST_COMMAND
    CST_COMMAND_UNIT,
    CST_CLASSIC_COMMAND,

    CST_COMMAND_PREFIX,
    CST_SCOPE_COMMAND,

    CST_NAME,
    CST_NAMES,

    CST_REDIRECT,
    CST_REDIRECT_IN,
    CST_REDIRECT_OUT,

    CST_CMD_SEP,
    CST_PIPE,
    CST_AMP,
    CST_NEW_CMD,

    CST_LITERAL,
    CST_DOUBLEQ,
    CST_SINGLEQ,

    CST_IF_STATEMENT,               // -> AST_IF_STATEMENT
    CST_IF_CONDITION_ACTION,
    CST_IF_ALTERNATIVE,
    CST_FOR_LOOP,                   // -> AST_FOR_LOOP
    CST_WHILE_LOOP,                 // -> AST_WHILE_LOOP
    CST_UNTIL_LOOP,                 // -> AST_UNTIL_LOOP
    CST_LOOP_BODY,
    CST_CASE_STATEMENT,             // -> AST_CASE_STATEMENT
    CST_CASE_EXPR,

    // Generated parsers
    CST_STRING_PARSER,
    CST_STRING_STATE_PARSER,
    CST_STATE_PARSER,

    // Auto-type
    CST_INVERTED = COMBINATOR_INVERTED_TYPE,
    CST_SEQUENCE = COMBINATOR_SEQUENCE_TYPE,
    CST_SEQUENCE_UNIT = COMBINATOR_SEQUENCE_UNIT_TYPE,
    CST_REPETITION = COMBINATOR_REPETITION_TYPE,
    CST_CHOICE = COMBINATOR_CHOICE_TYPE,
    CST_OPTIONAL = COMBINATOR_OPTIONAL_TYPE,

    CST_SEPARATED = COMBINATOR_SEPARATED_TYPE,
    CST_SEPARATED_REPETITION = COMBINATOR_SEPARATED_REPETITION_TYPE,

    // Volatile Parsers
    CST_GENERATOR = COMBINATOR_GENERATOR_TYPE,
    CST_LOOKAHEAD = COMBINATOR_LOOKAHEAD_TYPE,
} ConcreteNodeType;

extern const char * const CONCRETE_NODE_TYPE_STRING[];

typedef struct CSTNode {
    ConcreteNodeType type;
    Token * token;
    struct CSTNode ** children;
    size_t num_children;
} CSTNode;

void free_cst_node(CSTNode * node);
void free_cst_node_children(CSTNode node);
void prune_cst(CSTNode * node);

#endif // OCEAN_CST_H
