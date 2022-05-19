//
// Created on 16/05/2022.
//

#include <stdlib.h>
#include "parser/cst.h"

const char * const CONCRETE_NODE_TYPE_STRING[] = {
        [(ConcreteNodeType) NONE] = "none",
        [(ConcreteNodeType) COMMAND_ALL] = "command_all",
        [(ConcreteNodeType) COMMAND] = "command",
        [(ConcreteNodeType) COMMAND_TAIL] = "command_tail",
        [(ConcreteNodeType) NAME] = "name",
        [(ConcreteNodeType) NAMES] = "names",
        [(ConcreteNodeType) REDIRECT] = "redirect",
        [(ConcreteNodeType) REDIRECT_IN] = "redirect_in",
        [(ConcreteNodeType) REDIRECT_OUT] = "redirect_out",
        [(ConcreteNodeType) CMD_SEP] = "cmd_sep",
        [(ConcreteNodeType) PIPE] = "pipe",
        [(ConcreteNodeType) AMP] = "amp",
        [(ConcreteNodeType) NEW_CMD] = "new_cmd",
        [(ConcreteNodeType) LITERAL] = "literal",
        [(ConcreteNodeType) DOUBLEQ] = "doubleq",
        [(ConcreteNodeType) SINGLEQ] = "singleq",
        [(ConcreteNodeType) SEQUENCE] = "sequence",
        [(ConcreteNodeType) REPETITION] = "repetition",
        [(ConcreteNodeType) CHOICE] = "choice",
        [(ConcreteNodeType) OPTIONAL] = "optional",
};

void prune_cst(CSTNode * node)
{
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
        } else {
            prune_cst(child);
            i++;
        }
    }



}
