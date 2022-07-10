////
//// Created on 12/05/2022.
////
//
//#include <assert.h>
//#include <stdio.h>
//#include <string.h>
//#include "parser/ast.h"
//#include "eval/string_eval.h"
//
//#define PARENT_NODE_COMPLIANCE(node, parent_type, children_num) \
//assert((node).type == (parent_type)); \
//assert((node).num_children == (children_num));
//#define NODE_COMPLIANCE(node, parent_type, children_num, ...) \
//{ \
//    PARENT_NODE_COMPLIANCE(node, parent_type, children_num) \
//    const ParserType cnt_array[(children_num)] = {__VA_ARGS__}; \
//    for (int _node_compliance_arg_index = 0; _node_compliance_arg_index < (children_num); _node_compliance_arg_index++) \
//    { \
//        assert(node.children[_node_compliance_arg_index]->type == cnt_array[_node_compliance_arg_index]); \
//    } \
//}
//#define FLATTEN_CST_NODE(node, node_type) \
//{ \
//    assert((node).num_children == 1); \
//    assert((node).children[0]->type == (node_type)); \
//    (node).children[0]->type = (node).type; \
//    (node) = *(node).children[0];  \
//}
//#define FLATTEN_SEQ_UNIT(node) FLATTEN_CST_NODE(node, CST_SEQUENCE_UNIT)
//#define FLATTEN_CST_NODE_PARENT_VIEW(node, node_type) \
//{ \
//    assert((node).num_children == 1); \
//    assert((node).type == (node_type)); \
//    (node).children[0]->type = (node).type; \
//    (node) = *(node).children[0];  \
//}
//
//const char * const ABSTRACT_NODE_TYPE_STRING[] = {
//        [AST_NONE] = "none",
//
//        //
//        [AST_COMMAND_LIST] = "command_list",
//
//        [AST_COMMAND_SCOPE] = "command_scope",
//        [AST_COMMAND_CLASSIC] = "command_classic",
//
//        [AST_REDIRECTS] = "redirects",
//        [AST_REDIRECT_IN] = "redirect_in",
//        [AST_REDIRECT_OUT] = "redirect_out",
//
//        [AST_NAMES] = "names",
//        [AST_PREFIXES] = "prefixes",
//
//        //
//        [AST_IF_STATEMENT] = "if_statement",
//
//        [AST_FOR_LOOP] = "for",
//        [AST_WHILE_LOOP] = "while",
//        [AST_UNTIL_LOOP] = "until",
//
//        [AST_CASE_STATEMENT] = "case",
//        [AST_CASE_EXPR] = "case_expr",
//        [AST_CASE_MATCHES] = "case_matches",
//
//        [AST_CONDITIONAL_BRANCH] = "conditional_branch",
//
//        //
//        [AST_VALUE] = "value",
//        [AST_EVAL_VALUE] = "eval_value",
//
//        //
//        [AST_EMPTY] = "!empty",
//};
//
//static ASTNode abstract_cst_shell_instruction(CSTNode cst_node);
//static ASTNode abstract_cst_command(CSTNode cst_node);
//static ASTNode abstract_cst_command_redirect(CSTNode cst_node);
//static ASTNode abstract_cst_command_scope(CSTNode cst_node);
//static ASTNode abstract_cst_command_prefix(CSTNode cst_node);
//static ASTNode abstract_cst_command_classic(CSTNode cst_node);
//static ASTNode abstract_cst_names(CSTNode cst_node);
//static ASTNode abstract_cst_if_statement(CSTNode cst_node);
//static ASTNode abstract_cst_for_loop(CSTNode cst_node);
//static ASTNode abstract_cst_while_loop(CSTNode cst_node);
//static ASTNode abstract_cst_until_loop(CSTNode cst_node);
//static ASTNode abstract_cst_case_statement(CSTNode cst_node);
//static ASTNode abstract_cst_case_expression(CSTNode cst_node);
//static ASTNode abstract_cst_case_match(CSTNode cst_node);
//
//// Main
//ASTNode abstract_cst(CSTNode cst_node) {
//    NODE_COMPLIANCE(cst_node, NONE_PARSER, 1, CST_SHELL_INSTRUCTION)
//    return abstract_cst_shell_instruction(*cst_node.children[0]);
//}
//
//// Utils
//static ASTNode merge_cst_separated_rep(AbstractNodeType master_node_type, CSTNode cst_separated_rep, ASTNode (*converter)(CSTNode))
//{
//    assert(cst_separated_rep.num_children == 2);
//    assert(cst_separated_rep.children[1]->type == CST_SEPARATED_REPETITION);
//    CSTNode first = *cst_separated_rep.children[0];
//    CSTNode rest = *cst_separated_rep.children[1];
//
//    // 'first' + all the children in 'rest'
//    size_t num_ast_children = 1 + rest.num_children;
//    ASTNode * ast_children = reg_calloc(num_ast_children, sizeof(ASTNode));
//    ast_children[0] = converter(first);
//    for (size_t i = 1; i < num_ast_children; i++)
//        ast_children[i] = converter(*rest.children[i - 1]->children[1]);
//
//    return (ASTNode) {
//            .type = master_node_type,
//            .str = NULL,
//            .str_len = 0,
//            .children = ast_children,
//            .num_children = num_ast_children,
//    };
//}
//
//static ASTNode ast_value_with_str(char * str, size_t str_len, bool eval_str)
//{
//    ASTNode value = {
//        .type = eval_str ? AST_EVAL_VALUE : AST_VALUE,
//        .str = reg_malloc((str_len + 1)),
//        .str_len = str_len,
//        .num_children = 0,
//        .children = NULL,
//    };
//    strcpy(value.str, str);
//
//    return value;
//}
//
//static inline ASTNode ast_value(CSTNode cst_node, bool eval_str)
//{
//    eval_str = eval_str && \
//        cst_node.type == CST_LITERAL || \
//        cst_node.type == CST_DOUBLEQ || \
//        ( \
//                cst_node.token != NULL && \
//                cst_node.token->str != NULL && \
//                cst_node.token->str[0] == '"' && \
//                cst_node.token->str[cst_node.token->str_len - 1] == '"' \
//        );
//    return ast_value_with_str(cst_node.token->str, cst_node.token->str_len, eval_str);
//}
//
//static inline ASTNode empty_ast_node(void)
//{
//    return (ASTNode) { .type = AST_EMPTY };
//}
//
//// CST -> AST conversions
//static ASTNode abstract_cst_shell_instruction(CSTNode cst_node)
//{
//    NODE_COMPLIANCE(cst_node, CST_SHELL_INSTRUCTION, 2, CST_COMMAND, CST_SEPARATED_REPETITION)
//    return merge_cst_separated_rep(
//            AST_COMMAND_LIST,
//            cst_node,
//            abstract_cst_command);
//}
//
//#pragma clang diagnostic push
//#pragma ide diagnostic ignored "misc-no-recursion"
//static ASTNode abstract_cst_command(CSTNode cst_node) {
//    if (cst_node.num_children == 2) {
//        /*
//        NODE_COMPLIANCE(cst_node, CST_SEQUENCE_UNIT, 2, CST_CMD_SEP, CST_COMMAND)
//        return abstract_cst_command(*cst_node.children[1]);
//         */
//        // Should never reach here. But leave this in case ...
//        exit(1);
//    } else {
//        FLATTEN_SEQ_UNIT(cst_node)
//        NODE_COMPLIANCE(cst_node, CST_COMMAND, 2, CST_COMMAND_UNIT, CST_REDIRECT)
//    }
//    CSTNode cst_command_unit = *cst_node.children[0];
//    ASTNode command = {
//            .type = AST_NONE,
//            .str = NULL,
//            .str_len = 0,
//    };
//    PARENT_NODE_COMPLIANCE(cst_command_unit, CST_COMMAND_UNIT, 1)
//    if (cst_command_unit.children[0]->type == CST_SCOPE_COMMAND) {
//        command.type = AST_COMMAND_SCOPE;
//        command.children = reg_calloc(2, sizeof(ASTNode));
//        command.num_children = 2;
//        command.children[0] = abstract_cst_command_scope(*cst_command_unit.children[0]);
//        command.children[1] = abstract_cst_command_redirect(*cst_node.children[1]);
//    } else {
//        command = abstract_cst_command_classic(cst_command_unit);
//        command.children[2] = abstract_cst_command_redirect(*cst_node.children[1]);
//    }
//    return command;
//}
//#pragma clang diagnostic pop
//
//static ASTNode abstract_cst_command_classic(CSTNode cst_node) {
//    NODE_COMPLIANCE(cst_node, CST_COMMAND_UNIT, 1, CST_CLASSIC_COMMAND)
//    ASTNode command = empty_ast_node();
//    FLATTEN_CST_NODE(cst_node, CST_CLASSIC_COMMAND)
//    FLATTEN_SEQ_UNIT(cst_node)
//    NODE_COMPLIANCE(
//            cst_node,
//            CST_COMMAND_UNIT,
//            2,
//            CST_REPETITION,
//            // LOOKAHEAD
//            CST_NAMES)
//    command.type = AST_COMMAND_CLASSIC;
//    command.children = reg_calloc(3, sizeof(ASTNode));
//    command.num_children = 3;
//    command.children[0] = abstract_cst_command_prefix(*cst_node.children[0]);
//    command.children[1] = abstract_cst_names(*cst_node.children[1]);
//    command.children[2] = empty_ast_node();
//    return command;
//}
//
//static ASTNode abstract_cst_command_redirect_from_type(CSTNode cst_node, ParserType cst_redirect_type)
//{
//    assert(cst_node.type == CST_REDIRECT);
//    AbstractNodeType ast_redirect_type;
//    if (cst_redirect_type == CST_REDIRECT_IN)
//        ast_redirect_type = AST_REDIRECT_IN;
//    else if (cst_redirect_type == CST_REDIRECT_OUT)
//        ast_redirect_type = AST_REDIRECT_OUT;
//    else {
//        print_error(OCERR_EXIT, "Invalid cst redirect type: '%s'\n", CONCRETE_NODE_TYPE_STRING[cst_redirect_type]);
//        exit(1);
//    }
//    size_t num_redirects = 0;
//    ASTNode * redirects = reg_malloc(0);
//    for (size_t i = 0; i < cst_node.num_children; i++) {
//        CSTNode cst_redirect_node = *cst_node.children[i];
//        if (cst_redirect_node.type != cst_redirect_type)
//            continue;
//        // Right type of redirect
//        FLATTEN_SEQ_UNIT(cst_redirect_node)
//        NODE_COMPLIANCE(cst_redirect_node, cst_redirect_type, 2, CST_STATE_PARSER, CST_NAME)
//        CSTNode redirect_name = *cst_redirect_node.children[1];
//        PARENT_NODE_COMPLIANCE(redirect_name, CST_NAME, 1)
//        redirects = reg_realloc(redirects, 2 * (num_redirects + 1) * sizeof(ASTNode));
//        redirects[2 * num_redirects] = ast_value(*cst_redirect_node.children[0], false);
//        redirects[2 * num_redirects + 1] = ast_value(*redirect_name.children[0], true);
//        num_redirects++;
//    }
//    return (ASTNode) {
//        .type = ast_redirect_type,
//        .str = NULL,
//        .str_len = 0,
//        .children = redirects,
//        .num_children = 2 * num_redirects,
//    };
//}
//
//static ASTNode abstract_cst_command_redirect(CSTNode cst_node)
//{
//    assert(cst_node.type == CST_REDIRECT);
//    ASTNode redirections = {
//            .type = AST_REDIRECTS,
//            .str = NULL,
//            .str_len = 0,
//            .num_children = 2,
//            .children = reg_calloc(2, sizeof(ASTNode)),
//    };
//    redirections.children[0] = abstract_cst_command_redirect_from_type(cst_node, CST_REDIRECT_IN);
//    redirections.children[1] = abstract_cst_command_redirect_from_type(cst_node, CST_REDIRECT_OUT);
//    return redirections;
//}
//
//static ASTNode abstract_cst_command_scope(CSTNode cst_node)
//{
//    PARENT_NODE_COMPLIANCE(cst_node, CST_SCOPE_COMMAND, 1)
//    CSTNode cst_scope_cmd = *cst_node.children[0];
//    switch (cst_scope_cmd.type) {
//        case CST_IF_STATEMENT:
//            return abstract_cst_if_statement(cst_scope_cmd);
//        case CST_FOR_LOOP:
//            return abstract_cst_for_loop(cst_scope_cmd);
//        case CST_WHILE_LOOP:
//            return abstract_cst_while_loop(cst_scope_cmd);
//        case CST_UNTIL_LOOP:
//            return abstract_cst_until_loop(cst_scope_cmd);
//        case CST_CASE_STATEMENT:
//            return abstract_cst_case_statement(cst_scope_cmd);
//        default:
//            print_error(OCERR_EXIT, "Cannot abstract scope-command node of type '%s'\n", ABSTRACT_NODE_TYPE_STRING[cst_scope_cmd.type]);
//            exit(1);
//    }
//}
//
//static ASTNode abstract_cst_if_branch(CSTNode cst_node)
//{
//    FLATTEN_SEQ_UNIT(cst_node)
//    NODE_COMPLIANCE(
//            cst_node,
//            CST_IF_CONDITION_ACTION,
//            6,
//            CST_NAMES,
//            CST_NEW_CMD,
//            CST_STRING_PARSER,
//            CST_OPTIONAL,
//            CST_SHELL_INSTRUCTION,
//            CST_NEW_CMD)
//
//    ASTNode if_branch = {
//            .type = AST_CONDITIONAL_BRANCH,
//            .str = NULL,
//            .str_len = 0,
//            .num_children = 2,
//            .children = reg_calloc(2, sizeof(ASTNode)),
//    };
//    if_branch.children[0] = abstract_cst_names(*cst_node.children[0]);
//    if_branch.children[1] = abstract_cst_shell_instruction(*cst_node.children[4]);
//    return if_branch;
//}
//
//static ASTNode abstract_cst_if_statement(CSTNode cst_node)
//{
//    FLATTEN_SEQ_UNIT(cst_node)
//    NODE_COMPLIANCE(
//            cst_node,
//            CST_IF_STATEMENT,
//            4,
//            CST_STRING_PARSER,
//            CST_IF_CONDITION_ACTION,
//            CST_REPETITION,
//            CST_STRING_PARSER)
//
//    size_t ast_num_children = 1 + cst_node.children[2]->num_children;
//    ASTNode if_node = {
//            .type = AST_IF_STATEMENT,
//            .str = NULL,
//            .str_len = 0,
//            .num_children = ast_num_children,
//            .children = reg_calloc(ast_num_children, sizeof(ASTNode)),
//    };
//    // Add all the branches
//    if_node.children[0] = abstract_cst_if_branch(*cst_node.children[1]);
//    for (size_t i = 1; i < ast_num_children; i++)
//        if_node.children[i] = abstract_cst_if_branch(*cst_node.children[2]->children[i - 1]);
//
//    return if_node;
//}
//
//static ASTNode abstract_cst_loop_body(CSTNode cst_node)
//{
//    FLATTEN_SEQ_UNIT(cst_node)
//    NODE_COMPLIANCE(
//            cst_node,
//            CST_LOOP_BODY,
//            6,
//            CST_NEW_CMD,
//            CST_STRING_PARSER,
//            CST_OPTIONAL,
//            CST_SHELL_INSTRUCTION,
//            CST_NEW_CMD,
//            CST_STRING_PARSER)
//    return abstract_cst_shell_instruction(*cst_node.children[3]);
//}
//
//static ASTNode abstract_cst_for_loop(CSTNode cst_node)
//{
//    FLATTEN_SEQ_UNIT(cst_node)
//    NODE_COMPLIANCE(
//            cst_node,
//            CST_FOR_LOOP,
//            4,
//            CST_STRING_PARSER,
//            CST_LITERAL,
//            CST_OPTIONAL,
//            CST_LOOP_BODY)
//    ASTNode for_loop = {
//            .type = AST_FOR_LOOP,
//            .str = NULL,
//            .str_len = 0,
//            .num_children = 3,
//            .children = reg_calloc(3, sizeof(ASTNode)),
//    };
//    for_loop.children[0] = ast_value(*cst_node.children[1], false);
//    CSTNode cst_for_loop_in_names = *cst_node.children[2];
//    FLATTEN_SEQ_UNIT(cst_for_loop_in_names)
//    if (cst_for_loop_in_names.num_children == 0) {
//        for_loop.children[1] = empty_ast_node();
//    } else {
//        NODE_COMPLIANCE(cst_for_loop_in_names, CST_OPTIONAL, 2, CST_STRING_PARSER, CST_NAMES)
//        for_loop.children[1] = abstract_cst_names(*cst_for_loop_in_names.children[1]);
//    }
//    for_loop.children[2] = abstract_cst_loop_body(*cst_node.children[3]);
//
//    return for_loop;
//}
//
//static ASTNode abstract_cst_conditional_loop(CSTNode cst_node)
//{
//    ParserType loop_type = cst_node.type;
//    assert(loop_type == CST_WHILE_LOOP || loop_type == CST_UNTIL_LOOP);
//    NODE_COMPLIANCE(cst_node, loop_type, 1, CST_SEQUENCE_UNIT)
//    FLATTEN_SEQ_UNIT(cst_node)
//    NODE_COMPLIANCE(
//            cst_node,
//            loop_type,
//            3,
//            CST_STRING_PARSER,
//            CST_NAMES,
//            CST_LOOP_BODY)
//
//    ASTNode loop = {
//            .type = AST_NONE,
//            .str = NULL,
//            .str_len = 0,
//            .children = reg_calloc(2, sizeof(ASTNode)),
//            .num_children = 2,
//    };
//    loop.children[0] = abstract_cst_names(*cst_node.children[1]);
//    loop.children[1] = abstract_cst_loop_body(*cst_node.children[2]);
//    return loop;
//}
//
//static ASTNode abstract_cst_while_loop(CSTNode cst_node)
//{
//    ASTNode while_loop = abstract_cst_conditional_loop(cst_node);
//    while_loop.type = AST_WHILE_LOOP;
//    return while_loop;
//}
//
//static ASTNode abstract_cst_until_loop(CSTNode cst_node)
//{
//    ASTNode until_loop = abstract_cst_conditional_loop(cst_node);
//    until_loop.type = AST_UNTIL_LOOP;
//    return until_loop;
//}
//
//static ASTNode abstract_cst_case_statement(CSTNode cst_node)
//{
//    FLATTEN_SEQ_UNIT(cst_node)
//    NODE_COMPLIANCE(
//            cst_node,
//            CST_CASE_STATEMENT,
//            5,
//            CST_STRING_PARSER,
//            CST_LITERAL,
//            CST_STRING_PARSER,
//            CST_OPTIONAL,
//            CST_STRING_PARSER)
//
//    ASTNode case_statement = {
//            .type = AST_CASE_STATEMENT,
//            .str = NULL,
//            .str_len = 0,
//            .children = reg_calloc(2, sizeof(ASTNode)),
//            .num_children = 2,
//    };
//    ASTNode case_word = ast_value(*cst_node.children[1], false);
//    case_statement.children[0] = case_word;
//    CSTNode cst_case_expressions = *cst_node.children[3];
//    PARENT_NODE_COMPLIANCE(cst_case_expressions, CST_OPTIONAL, 1)
//    cst_case_expressions = *cst_case_expressions.children[0];
//    if (cst_case_expressions.type == CST_STRING_STATE_PARSER) {
//        assert(cst_case_expressions.token != NULL);
//        assert(cst_case_expressions.token->str != NULL);
//        assert(cst_case_expressions.token->str_len == 1);
//        assert(cst_case_expressions.token->str[0] == '\n' && cst_case_expressions.token->str[1] == 0);
//        case_statement.children[1] = empty_ast_node();
//        return case_statement;
//    }
//    // There are some case expressions
//    FLATTEN_SEQ_UNIT(cst_case_expressions)
//    NODE_COMPLIANCE(
//            cst_case_expressions,
//            CST_SEQUENCE,
//            2,
//            CST_SEPARATED,
//            CST_CHOICE)
//    cst_case_expressions = *cst_case_expressions.children[0];
//    return merge_cst_separated_rep(
//            AST_CASE_EXPR,
//            cst_case_expressions,
//            abstract_cst_case_expression);
//}
//
//static ASTNode abstract_cst_case_expression(CSTNode cst_node)
//{
//    FLATTEN_SEQ_UNIT(cst_node)
//    NODE_COMPLIANCE(
//            cst_node,
//            CST_CASE_EXPR,
//            6,
//            CST_OPTIONAL,
//            CST_OPTIONAL,
//            CST_SEPARATED,
//            CST_STRING_PARSER,
//            CST_OPTIONAL,
//            CST_SHELL_INSTRUCTION)
//    ASTNode case_expr = {
//            .type = AST_CONDITIONAL_BRANCH,
//            .str = NULL,
//            .str_len = 0,
//            .num_children = 2,
//            .children = reg_calloc(2, sizeof(ASTNode)),
//    };
//    CSTNode cst_case_matches = *cst_node.children[2];
//    NODE_COMPLIANCE(cst_case_matches, CST_SEPARATED, 2, CST_NAME, CST_SEPARATED_REPETITION)
//    ASTNode case_matches = merge_cst_separated_rep(
//            AST_CASE_MATCHES,
//            cst_case_matches,
//            abstract_cst_case_match);
//    ASTNode case_instructions = abstract_cst_shell_instruction(*cst_node.children[5]);
//    case_expr.children[0] = case_matches;
//    case_expr.children[1] = case_instructions;
//    return case_expr;
//}
//
//static ASTNode abstract_cst_case_match(CSTNode cst_node)
//{
//    NODE_COMPLIANCE(cst_node, CST_NAME, 1, CST_LITERAL)
//    FLATTEN_CST_NODE(cst_node, CST_LITERAL)
//    return ast_value(cst_node, false);
//}
//
//static ASTNode abstract_cst_command_prefix(CSTNode cst_node)
//{
//    assert(cst_node.type == CST_REPETITION);
//    if (cst_node.num_children == 0)
//        return empty_ast_node();
//    else
//        for (size_t i = 0; i < cst_node.num_children; i++)
//            assert(cst_node.children[i]->type == CST_SEQUENCE_UNIT);
//    // Two times: one for key, one for value
//    size_t num_prefixes = 2 * cst_node.num_children;
//    ASTNode prefixes = {
//            .type = AST_PREFIXES,
//            .str  ="",
//            .str_len = 0,
//            .num_children = num_prefixes,
//            .children = reg_calloc(num_prefixes, sizeof(ASTNode)),
//    };
//    for (size_t i = 0, prefix_index = 0; i < cst_node.num_children; i++, prefix_index += 2) {
//        CSTNode prefix_key = *cst_node.children[i]->children[0];
//        CSTNode prefix_value = *cst_node.children[i]->children[2];
//        PARENT_NODE_COMPLIANCE(prefix_value, CST_NAME, 1)
//        prefix_value = *prefix_value.children[0];
//        prefixes.children[prefix_index] = ast_value_with_str(prefix_key.token->str, prefix_key.token->str_len, false);
//        prefixes.children[prefix_index + 1] = ast_value(prefix_value, true);
//    }
//
//    return prefixes;
//}
//
//static ASTNode abstract_cst_names_eval(CSTNode cst_node)
//{
//    assert(cst_node.type == CST_NAMES);
//    CSTNode repetition = *cst_node.children[1];
//    size_t num_names = 1 + repetition.num_children;
//    ASTNode names = {
//            .type = AST_NAMES,
//            .str = "",
//            .str_len = 0,
//            .num_children = num_names,
//            .children = reg_calloc(num_names, sizeof(ASTNode)),
//    };
//    // Fill the names
//    CSTNode first_name = *cst_node.children[0];
//    assert(first_name.type == CST_NAME);
//    assert(first_name.num_children == 1);
//    first_name = *first_name.children[0];
//    names.children[0] = ast_value(first_name, true);
//    for (size_t i = 0; i < repetition.num_children; i++) {
//        CSTNode name_cst_node = *repetition.children[i];
//        names.children[i + 1] = ast_value(name_cst_node, true);
//    }
//
//    return names;
//}
//
//static ASTNode abstract_cst_names(CSTNode cst_node)
//{
//    FLATTEN_SEQ_UNIT(cst_node)
//    return abstract_cst_names_eval(cst_node);
//}
