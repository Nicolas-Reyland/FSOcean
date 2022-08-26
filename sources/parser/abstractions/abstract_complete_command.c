//
// Created by nicolas on 22. 8. 26.
//

#include "parser/abstractions/abstract_complete_command.h"
#include "parser/abstractions/abstraction.h"
#include "parser/abstractions/abstract_and_or.h"

Executable abstract_complete_command(CSTNode cst_node) {
    NODE_COMPLIANCE(cst_node, COMPLETE_COMMAND_PARSER, 2, LIST_PARSER, OPTIONAL_PARSER)
    // We don't care about the optional parser, since if it's there, it's like a "double end" for the command
    cst_node = *cst_node.children[0];
    NODE_COMPLIANCE(cst_node, LIST_PARSER, 2, AND_OR_PARSER, SEPARATOR_OP_PARSER);
    // Once again, we don't care about the
    cst_node = *cst_node.children[0];
    return abstract_and_or(cst_node);
}
