//
// Created by nicolas on 22. 8. 26.
//

#include "parser/abstractions/abstract_complete_commands.h"
#include "parser/abstractions/abstract_complete_command.h"

Executable abstract_complete_commands(CSTNode cst_node) {
    NODE_COMPLIANCE(cst_node, COMPLETE_COMMANDS_PARSER, 2, COMPLETE_COMMAND_PARSER, SEPARATED_REPETITION_PARSER)
    Executable first_command = abstract_complete_command(*cst_node.children[0]);
    if (!has_children(*cst_node.children[1]))
        return first_command;
    // multi commands
    size_t num_children = cst_node.children[1]->num_children + 1;
    struct ExecMultiExecutables multi = {
            .execution_flags = 0,
            .num_executables = num_children,
            .executables = calloc(num_children, sizeof(struct Executable)),
    };
    multi.executables[0] = first_command;
    CSTNode sep_rep_node = *cst_node.children[1];
    for (size_t i = 0; i < sep_rep_node.num_children; i++)
        multi.executables[i + 1] = abstract_complete_command(*sep_rep_node.children[i]);

    return (struct Executable) {
            .type = EXEC_MULTI,
            .executable = (union ExecutableUnion) {
                    .multi = multi,
            },
            .redirect = false,
    };
}
