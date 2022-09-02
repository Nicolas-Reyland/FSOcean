//
// Created by Nicolas Reyland on 8/29/22.
//

#include "parser/abstractions/compound_command/abstract_while_clause.h"
#include "parser/abstractions/abstraction.h"
#include "parser/abstractions/compound_command/imperfect_abstract_compound_list.h"

Executable abstract_while_clause(CSTNode while_clause) {
    NODE_COMPLIANCE(while_clause, WHILE_CLAUSE_PARSER, 3, GEN_STRING_R1_PARSER, COMPOUND_LIST_PARSER, DO_GROUP_PARSER)
    CSTNode do_group = *while_clause.children[2];
    NODE_COMPLIANCE(do_group, DO_GROUP_PARSER, 3, GEN_STRING_R1_PARSER, COMPOUND_LIST_PARSER, GEN_STRING_R1_PARSER)
    size_t num_condition_executables = 0,
           num_body_executables = 0;
    Executable * condition_executables = imperfect_abstract_compound_list(*while_clause.children[1], &num_condition_executables),
               * body_executables = imperfect_abstract_compound_list(*do_group.children[1], &num_body_executables);
    return (Executable) {
            .type = EXEC_WHILE_LOOP,
            .executable = (union ExecutableUnion) {
                    .while_loop = (struct ExecWhileLoop) {
                            .num_condition_executables = num_condition_executables,
                            .condition_executables = condition_executables,
                            .body_executables = body_executables,
                            .num_body_executables = num_body_executables,
                    },
            },
    };
}
