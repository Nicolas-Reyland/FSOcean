//
// Created on 12/05/2022.
//

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser/ast.h"
#include "eval/shellstring.h"

#define PARENT_NODE_COMPLIANCE(node, parent_type, children_num) \
assert(node.type == (parent_type)); \
assert(node.num_children == (children_num));
#define NODE_COMPLIANCE(node, parent_type, children_num, ...) \
{ \
    PARENT_NODE_COMPLIANCE(node, parent_type, children_num) \
    const ConcreteNodeType cnt_array[(children_num)] = {__VA_ARGS__}; \
    for (int _node_compliance_arg_index = 0; _node_compliance_arg_index < (children_num); _node_compliance_arg_index++) \
    { \
        assert(node.children[_node_compliance_arg_index]->type == cnt_array[_node_compliance_arg_index]); \
    } \
}
#define FLATTEN_CST_NODE(node, node_type) \
{ \
    assert(node.num_children == 1); \
    assert(node.children[0]->type == node_type); \
    node.children[0]->type = node.type; \
    node = *node.children[0];  \
}
#define FLATTEN_SEQ_UNIT(node) FLATTEN_CST_NODE(node, CST_SEQUENCE_UNIT)
#define FLATTEN_CST_NODE_PARENT_VIEW(node, node_type) \
{ \
    assert(node.num_children == 1); \
    assert(node.type == node_type); \
    node.children[0]->type = node.type; \
    node = *node.children[0];  \
}

const char * const ABSTRACT_NODE_TYPE_STRING[] = {
        [AST_NONE] = "none",

        //
        [AST_COMMAND_LIST] = "command_list",

        [AST_COMMAND_SCOPE] = "command_scope",
        [AST_COMMAND_CLASSIC] = "command_classic",

        [AST_NAMES] = "names",
        [AST_PREFIXES] = "prefixes",

        //
        [AST_IF_STATEMENT] = "if_statement",
        [AST_CONDITIONAL_BRANCH] = "conditional_branch",

        [AST_FOR_LOOP] = "for",
        [AST_WHILE_LOOP] = "while",
        [AST_UNTIL_LOOP] = "until",
        [AST_CASE_STATEMENT] = "case",

        //
        [AST_VALUE] = "value",
};

static ASTNode abstract_cst_shell_instruction(CSTNode cst_node);
static ASTNode abstract_cst_command(CSTNode cst_node);
static ASTNode abstract_cst_command_redirect(CSTNode cst_node);
static ASTNode abstract_cst_command_scope(CSTNode cst_node);
static ASTNode abstract_cst_command_prefix(CSTNode cst_node);
static inline ASTNode abstract_cst_names(CSTNode cst_node);
static inline ASTNode abstract_cst_names_raw(CSTNode cst_node);
static ASTNode abstract_cst_if_statement(CSTNode cst_node);
static ASTNode abstract_cst_for_loop(CSTNode cst_node);
static ASTNode abstract_cst_while_loop(CSTNode cst_node);
static ASTNode abstract_cst_until_loop(CSTNode cst_node);
static ASTNode abstract_cst_case_statement(CSTNode cst_node);

// Main
ASTNode abstract_cst(CSTNode cst_node) {
    NODE_COMPLIANCE(cst_node, CST_NONE, 1, CST_SHELL_INSTRUCTION)
    return abstract_cst_shell_instruction(*cst_node.children[0]);
}

// Utils
static ASTNode merge_cst_separated_rep(AbstractNodeType type, CSTNode cst_separated_rep, ASTNode (*converter)(CSTNode))
{
    CSTNode first = *cst_separated_rep.children[0];
    CSTNode rest = *cst_separated_rep.children[1];

    // 'first' + all the children in 'rest'
    size_t num_ast_children = 1 + rest.num_children;
    ASTNode * ast_children = calloc(num_ast_children, sizeof(ASTNode));
    ast_children[0] = converter(first);
    for (size_t i = 1; i < num_ast_children; i++)
        ast_children[i] = converter(*rest.children[i - 1]);

    return (ASTNode) {
            .type = type,
            .str = NULL,
            .str_len = 0,
            .children = ast_children,
            .num_children = num_ast_children,
    };
}

static ASTNode ast_value_with_str(char * str, size_t str_len, bool eval_str)
{
    ASTNode value = {
        .type = AST_VALUE,
        .str = malloc(str_len * sizeof(char)),
        .str_len = str_len,
        .num_children = 0,
        .children = NULL,
    };
    strcpy(value.str, str);
    // eval string
    if (eval_str)
        value.str_len = eval_double_quoted_string(&value.str, value.str_len);

    return value;
}

static inline ASTNode ast_value(CSTNode cst_node, bool eval_str)
{
    return ast_value_with_str(cst_node.token->str, cst_node.token->str_len, eval_str);
}

static inline ASTNode empty_ast_node(void)
{
    return (ASTNode) { .type = AST_NONE };
}

// CST -> AST conversions
static ASTNode abstract_cst_shell_instruction(CSTNode cst_node)
{
    NODE_COMPLIANCE(cst_node, CST_SHELL_INSTRUCTION, 2, CST_COMMAND, CST_SEPARATED_REPETITION)
    return merge_cst_separated_rep(
            AST_COMMAND_LIST,
            cst_node,
            abstract_cst_command);
}

static ASTNode abstract_cst_command(CSTNode cst_node) {
    FLATTEN_SEQ_UNIT(cst_node)
    NODE_COMPLIANCE(cst_node, CST_COMMAND, 2, CST_COMMAND_UNIT, CST_REPETITION)
    CSTNode cst_command_unit = *cst_node.children[0];
    ASTNode command = {
            .type = AST_NONE,
            .str = NULL,
            .str_len = 0,
    };
    PARENT_NODE_COMPLIANCE(cst_command_unit, CST_COMMAND_UNIT, 1)
    if (cst_command_unit.children[0]->type == CST_SCOPE_COMMAND) {
        command.type = AST_COMMAND_SCOPE;
        command.children = calloc(2, sizeof(ASTNode));
        command.num_children = 2;
        command.children[0] = abstract_cst_command_scope(*cst_command_unit.children[0]);
        command.children[1] = abstract_cst_command_redirect(*cst_node.children[1]);
    } else {
        FLATTEN_CST_NODE_PARENT_VIEW(cst_command_unit, CST_COMMAND_UNIT)
        FLATTEN_SEQ_UNIT(cst_command_unit)
        command.type = AST_COMMAND_CLASSIC;
        command.children = calloc(3, sizeof(ASTNode));
        command.num_children = 3;
        command.children[0] = abstract_cst_command_prefix(*cst_command_unit.children[0]);
        command.children[1] = abstract_cst_names(*cst_command_unit.children[1]);
        command.children[2] = abstract_cst_command_redirect(*cst_node.children[1]);
    }
    return command;
}

static ASTNode abstract_cst_command_redirect(CSTNode cst_node)
{
    return empty_ast_node();
}

static ASTNode abstract_cst_command_scope(CSTNode cst_node)
{
    PARENT_NODE_COMPLIANCE(cst_node, CST_SCOPE_COMMAND, 1)
    CSTNode cst_scope_cmd = *cst_node.children[0];
    switch (cst_scope_cmd.type) {
        case CST_IF_STATEMENT:
            return abstract_cst_if_statement(cst_scope_cmd);
        case CST_FOR_LOOP:
            return abstract_cst_for_loop(cst_scope_cmd);
        case CST_WHILE_LOOP:
            return abstract_cst_while_loop(cst_scope_cmd);
        case CST_UNTIL_LOOP:
            return abstract_cst_until_loop(cst_scope_cmd);
        case CST_CASE_STATEMENT:
            return abstract_cst_case_statement(cst_scope_cmd);
        default:
            fprintf(stderr, "Cannot abstract scope-command node of type '%s'\n", ABSTRACT_NODE_TYPE_STRING[cst_scope_cmd.type]);
            exit(1);
    }
}

static ASTNode abstract_cst_if_branch(CSTNode cst_node)
{
    NODE_COMPLIANCE(
            cst_node,
            CST_IF_CONDITION_ACTION,
            6,
            CST_NAMES,
            CST_NEW_CMD,
            CST_STRING_PARSER,
            CST_OPTIONAL,
            CST_SHELL_INSTRUCTION,
            CST_NEW_CMD)

    ASTNode if_branch = {
            .type = AST_CONDITIONAL_BRANCH,
            .str = NULL,
            .str_len = 0,
            .num_children = 2,
            .children = calloc(2, sizeof(ASTNode)),
    };
    if_branch.children[0] = abstract_cst_names(*cst_node.children[0]);
    if_branch.children[1] = abstract_cst_shell_instruction(*cst_node.children[4]);
    return if_branch;
}

static ASTNode abstract_cst_if_statement(CSTNode cst_node)
{
    NODE_COMPLIANCE(
            cst_node,
            CST_IF_STATEMENT,
            4,
            CST_STRING_PARSER,
            CST_IF_CONDITION_ACTION,
            CST_REPETITION,
            CST_STRING_PARSER)

    size_t ast_num_children = 1 + cst_node.children[2]->num_children;
    ASTNode if_node = {
            .type = AST_IF_STATEMENT,
            .str = NULL,
            .str_len = 0,
            .num_children = ast_num_children,
            .children = calloc(ast_num_children, sizeof(ASTNode)),
    };
    // Add all the branches
    if_node.children[0] = abstract_cst_if_branch(*cst_node.children[1]);
    for (size_t i = 1; i < ast_num_children; i++)
        if_node.children[i] = abstract_cst_if_branch(*cst_node.children[2]->children[i - 1]);

    return if_node;
}

static ASTNode abstract_loop_body(CSTNode cst_node)
{
    FLATTEN_SEQ_UNIT(cst_node)
    NODE_COMPLIANCE(
            cst_node,
            CST_LOOP_BODY,
            6,
            CST_NEW_CMD,
            CST_STRING_PARSER,
            CST_OPTIONAL,
            CST_SHELL_INSTRUCTION,
            CST_NEW_CMD,
            CST_STRING_PARSER)
    return abstract_cst_shell_instruction(*cst_node.children[3]);
}

static ASTNode abstract_cst_for_loop(CSTNode cst_node)
{
    FLATTEN_SEQ_UNIT(cst_node)
    NODE_COMPLIANCE(
            cst_node,
            CST_FOR_LOOP,
            4,
            CST_STRING_PARSER,
            CST_LITERAL,
            CST_OPTIONAL,
            CST_LOOP_BODY)
    ASTNode for_loop = {
            .type = AST_FOR_LOOP,
            .str = NULL,
            .str_len = 0,
            .num_children = 3,
            .children = calloc(3, sizeof(ASTNode)),
    };
    for_loop.children[0] = ast_value(*cst_node.children[1], false);
    CSTNode cst_for_loop_in_names = *cst_node.children[2];
    FLATTEN_SEQ_UNIT(cst_for_loop_in_names)
    if (cst_for_loop_in_names.num_children == 0) {
        for_loop.children[1] = empty_ast_node();
    } else {
        NODE_COMPLIANCE(cst_for_loop_in_names, CST_OPTIONAL, 2, CST_STRING_PARSER, CST_NAMES)
        for_loop.children[1] = abstract_cst_names(*cst_for_loop_in_names.children[1]);
    }
    for_loop.children[2] = abstract_loop_body(*cst_node.children[3]);

    return for_loop;
}

static ASTNode abstract_cst_while_loop(CSTNode cst_node)
{
    return (ASTNode) {};
}

static ASTNode abstract_cst_until_loop(CSTNode cst_node)
{
    return (ASTNode) {};
}

static ASTNode abstract_cst_case_statement(CSTNode cst_node)
{
    return (ASTNode) {};
}

static ASTNode abstract_cst_command_prefix(CSTNode cst_node)
{
    assert(cst_node.type == CST_COMMAND_PREFIX);
    if (cst_node.num_children == 0)
        return empty_ast_node();
    else
        NODE_COMPLIANCE(cst_node, 1, CST_COMMAND_PREFIX, CST_REPETITION)
    CSTNode cst_repetition = *cst_node.children[0];
    // Two times: one for key, one for value
    size_t num_prefixes = 2 * cst_repetition.num_children;
    ASTNode prefixes = {
            .type = AST_PREFIXES,
            .str  ="",
            .str_len = 0,
            .num_children = num_prefixes,
            .children = calloc(num_prefixes, sizeof(ASTNode)),
    };
    for (size_t i = 0, prefix_index = 0; i < cst_repetition.num_children; i++, prefix_index += 2) {
        CSTNode prefix_key = *cst_repetition.children[i]->children[0];
        CSTNode prefix_value = *cst_repetition.children[i]->children[2];
        prefixes.children[prefix_index] = ast_value(prefix_key, false);
        prefixes.children[prefix_index + 1] = ast_value(prefix_value, true);
    }

    return prefixes;
}

static ASTNode abstract_cst_names_eval(CSTNode cst_node, bool eval)
{
    assert(cst_node.type == CST_NAMES);
    CSTNode repetition = *cst_node.children[1];
    size_t num_names = 1 + repetition.num_children;
    ASTNode names = {
            .type = AST_NAMES,
            .str = "",
            .str_len = 0,
            .num_children = num_names,
            .children = calloc(num_names, sizeof(ASTNode)),
    };
    // Fill the names
    CSTNode first_name = *cst_node.children[0];
    FLATTEN_CST_NODE_PARENT_VIEW(first_name, CST_NAME)
    names.children[0] = ast_value(first_name, eval);
    for (size_t i = 0; i < repetition.num_children; i++) {
        CSTNode name_cst_node = *repetition.children[i];
        // FLATTEN_CST_NODE_PARENT_VIEW(name_cst_node, CST_NAME)
        names.children[i + 1] = ast_value(name_cst_node, eval);
    }

    return names;
}

static ASTNode abstract_cst_names(CSTNode cst_node)
{
    FLATTEN_SEQ_UNIT(cst_node)
    return abstract_cst_names_eval(cst_node, true);
}

static inline ASTNode abstract_cst_names_raw(CSTNode cst_node)
{
    FLATTEN_SEQ_UNIT(cst_node)
    return abstract_cst_names_eval(cst_node, false);
}
