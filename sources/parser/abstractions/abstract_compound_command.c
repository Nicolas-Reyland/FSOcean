//
// Created by Nicolas Reyland on 8/28/22.
//

#include "parser/abstractions/abstract_compound_command.h"
#include "misc/output/output.h"
#include "parser/abstractions/compound_command/abstract_brace_group.h"
#include "parser/abstractions/compound_command/abstract_subshell.h"
#include "parser/abstractions/compound_command/abstract_for_clause.h"
#include "parser/abstractions/compound_command/abstract_while_clause.h"
#include "parser/abstractions/compound_command/abstract_until_clause.h"
#include "parser/abstractions/compound_command/abstract_case_clause.h"
#include "parser/abstractions/compound_command/abstract_if_clause.h"
#include "parser/abstractions/abstraction.h"

Executable abstract_compound_command(CSTNode cst_node)
{
    PARENT_NODE_COMPLIANCE(cst_node, COMPOUND_COMMAND_PARSER, 1)
    cst_node = *cst_node.children[0];
    switch (cst_node.type) {
        case BRACE_GROUP_PARSER:
            return abstract_brace_group(cst_node);
        case SUBSHELL_PARSER:
            return abstract_subshell(cst_node);
        case FOR_CLAUSE_PARSER:
            return abstract_for_clause(cst_node);
        case WHILE_CLAUSE_PARSER:
            return abstract_while_clause(cst_node);
        case UNTIL_CLAUSE_PARSER:
            return abstract_until_clause(cst_node);
        case CASE_CLAUSE_PARSER:
            return abstract_case_clause(cst_node);
        case IF_CLAUSE_PARSER:
            return abstract_if_clause(cst_node);
        default:
            print_error_exit("Unexpected type for compound-command: '%d'\n", cst_node.type);
    }
}
