//
// Created on 16/05/2022.
//

#include <stdlib.h>
#include "parser/cst.h"

const char * const CONCRETE_NODE_TYPE_STRING[] = {
        [CST_NONE] = "none",
        [CST_SHELL_INSTRUCTION] = "shell_instruction",

        [CST_COMMAND] = "command",
        [CST_COMMAND_UNIT] = "command_unit",
        [CST_CLASSIC_COMMAND] = "classic_command",

        [CST_COMMAND_PREFIX] = "command_prefix",
        [CST_SCOPE_COMMAND] = "scope_command",

        [CST_NAME] = "name",
        [CST_NAMES] = "names",

        [CST_REDIRECT] = "redirect",
        [CST_REDIRECT_IN] = "redirect_in",
        [CST_REDIRECT_OUT] = "redirect_out",

        [CST_CMD_SEP] = "cmd_sep",
        [CST_PIPE] = "pipe",
        [CST_AMP] = "amp",
        [CST_NEW_CMD] = "new_cmd",

        [CST_LITERAL] = "literal",
        [CST_DOUBLEQ] = "doubleq",
        [CST_SINGLEQ] = "singleq",

        [CST_IF_STATEMENT] = "if_statement",
        [CST_IF_CONDITION_ACTION] = "if_condition_action_parser",
        [CST_IF_ALTERNATIVE] = "if_alternative_parser",
        [CST_FOR_LOOP] = "for_loop",
        [CST_WHILE_LOOP] = "while_loop",
        [CST_UNTIL_LOOP] = "until_loop",
        [CST_LOOP_BODY] = "loop_body",
        [CST_CASE_STATEMENT] = "case_statement",
        [CST_CASE_EXPR] = "case_expr",

        [CST_INVERTED] = "inverted",
        [CST_SEQUENCE] = "sequence",
        [CST_SEQUENCE_UNIT] = "seq_unit",
        [CST_REPETITION] = "repetition",
        [CST_CHOICE] = "choice",
        [CST_OPTIONAL] = "optional",

        [CST_SEPARATED] = "separated",
        [CST_SEPARATED_REPETITION] = "separated_repetition",

        [CST_GENERATOR] = "generator",
        [CST_LOOKAHEAD] = "lookahead",

        [CST_STRING_PARSER] = "#string",
        [CST_STRING_STATE_PARSER] = "#string_state",
        [CST_STATE_PARSER] = "#type",
};


#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
void free_cst_node(CSTNode * node) {
    if (node->num_children != 0) {
        for (size_t i = 0; i < node->num_children; i++) {
            free_cst_node(node->children[i]);
        }
    }
    free(node);
}

void free_cst_node_children(CSTNode node) {
    if (node.num_children != 0) {
        for (size_t i = 0; i < node.num_children; i++) {
            free_cst_node(node.children[i]);
        }
    }
}
void prune_cst(CSTNode *node) {
    // cannot prune itself from tree
    if (node->num_children == 0)
        return;
    // prune the meaningless children
    for (size_t i = 0; i < node->num_children; ) {
        CSTNode * child = node->children[i];
        if ((child->num_children == 0 || child->children == NULL) && child->token == NULL) {
            // remove child
            node->num_children--;
            for (size_t j = i; j < node->num_children; j++)
                node->children[j] = node->children[j + 1];
            node->children = realloc(node->children, node->num_children * sizeof(CSTNode *));
            free(child);
        } else if (child->type == CST_GENERATOR && child->num_children == 1) {
            node->children[i] = child->children[0];
            free(child);
        } else {
            prune_cst(child);
            i++;
        }
    }
}
#pragma clang diagnostic pop
