//
// Created on 12/05/2022.
//

#include <assert.h>
#include "parser/abstraction.h"

#define PARENT_NODE_COMPLIANCE(node, parent_type, children_num) \
assert((node).type == (parent_type)); \
assert((node).num_children == (children_num));

#define NODE_COMPLIANCE(node, parent_type, children_num, ...) \
{ \
    PARENT_NODE_COMPLIANCE(node, parent_type, children_num) \
    const ParserType cnt_array[(children_num)] = {__VA_ARGS__}; \
    for (int _node_compliance_arg_index = 0; _node_compliance_arg_index < (children_num); _node_compliance_arg_index++) \
    { \
        assert(node.children[_node_compliance_arg_index]->type == cnt_array[_node_compliance_arg_index]); \
    } \
}

#define FLATTEN_CST_NODE(node, node_type) \
{ \
    assert((node).num_children == 1); \
    assert((node).children[0]->type == (node_type)); \
    (node).children[0]->type = (node).type; \
    (node) = *(node).children[0];  \
}

#define FLATTEN_CST_NODE_PARENT_VIEW(node, node_type) \
{ \
    assert((node).num_children == 1); \
    assert((node).type == (node_type)); \
    (node).children[0]->type = (node).type; \
    (node) = *(node).children[0];  \
}

static Executable abstract_complete_commands(CSTNode cst_node);
static bool optional_exist(CSTNode cst_node, bool assert_type);

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
    if (!optional_exist(*cst.children[1], true)) {
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

Executable abstract_complete_commands(CSTNode cst_node) {
    NODE_COMPLIANCE(cst_node, COMPLETE_COMMANDS_PARSER, 2, COMPLETE_COMMAND_PARSER, SEPARATED_REPETITION_PARSER)

    Executable commands;

















}

bool optional_exist(CSTNode cst_node, bool assert_type) {
    if (assert_type)
        assert(cst_node.type == OPTIONAL_PARSER);
    return cst_node.num_children != 0;
}
