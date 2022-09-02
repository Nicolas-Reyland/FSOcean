//
// Created on 12/05/2022.
//

#ifndef OCEAN_ABSTRACTION_H
#define OCEAN_ABSTRACTION_H

#include <assert.h>
#include <stddef.h>
#include "parser/cst.h"
#include "executable/executable.h"

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

Executable abstract_cst(CSTNode cst_node);

inline bool has_children(CSTNode cst_node) {
    return (cst_node.num_children != 0 && cst_node.children != NULL);
}

#endif // OCEAN_ABSTRACTION_H
