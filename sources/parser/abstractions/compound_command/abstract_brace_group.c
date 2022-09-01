//
// Created by Nicolas Reyland on 8/29/22.
//

#include "parser/abstractions/compound_command/abstract_brace_group.h"
#include "parser/abstractions/abstraction.h"
#include "parser/abstractions/compound_command/imperfect_abstract_compound_list.h"

Executable abstract_brace_group(CSTNode brace_group) {
    NODE_COMPLIANCE(brace_group, BRACE_GROUP_PARSER, 3, GEN_STRING_PARSER, COMPOUND_LIST_PARSER, GEN_STRING_PARSER)
    size_t num_executables = 0;
    Executable * executables = imperfect_abstract_compound_list(*brace_group.children[1], &num_executables);
    return create_exec_multi_executables(EXE_BRACE_GROUP, executables, num_executables);
}
