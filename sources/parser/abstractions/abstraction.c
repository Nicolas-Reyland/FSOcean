//
// Created on 12/05/2022.
//

#include "parser/abstractions/abstraction.h"
#include "parser/abstractions/abstract_complete_commands.h"

extern inline bool has_children(CSTNode cst_node);

Executable abstract_cst(CSTNode cst)
{
    // Check existence of the parent-of-all NONE-PARSER
    NODE_COMPLIANCE(cst, NONE_PARSER, 1, PROGRAM_PARSER)
    cst = *cst.children[0]; // to PROGRAM_PARSER

    /*
     * If there is an actual command, there will be lots of unique parers lying around.
     * Abstracting these away here, manually, is easier that creating custom
     * abstraction functions that will be called once anyway...
     */
    NODE_COMPLIANCE(cst, PROGRAM_PARSER, 2, LINEBREAK_PARSER, OPTIONAL_PARSER)
    if (!has_children(*cst.children[1])) {
        // there is no actual command, just a linebreak
        return empty_exec_command();
    }
    // There is a command following
    cst = *cst.children[1]; // to OPTIONAL_PARSER
    FLATTEN_CST_NODE(cst, SEQUENCE_PARSER) // to SEQUENCE_PARSER
    NODE_COMPLIANCE(cst, OPTIONAL_PARSER, 2, COMPLETE_COMMANDS_PARSER, LINEBREAK_PARSER)
    cst = *cst.children[0]; // to COMPLETE_COMMANDS_PARSER

    return abstract_complete_commands(cst);
}
