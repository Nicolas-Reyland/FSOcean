//
// Created by Nicolas Reyland on 8/29/22.
//

#include "parser/abstractions/compound_command/abstract_if_clause.h"
#include "parser/abstractions/abstraction.h"
#include "parser/abstractions/compound_command/imperfect_abstract_compound_list.h"
#include "misc/safemem.h"

static Executable * imperfect_abstract_else_part(CSTNode else_part, size_t * num_executables);

Executable abstract_if_clause(CSTNode if_clause)
{
    NODE_COMPLIANCE(if_clause, IF_CLAUSE_PARSER, 6,
                    GEN_STRING_R1_PARSER, /* If : 0 */
                    COMPOUND_LIST_PARSER, /* Condition : 1 */
                    GEN_STRING_R1_PARSER, /* Then : 2 */
                    COMPOUND_LIST_PARSER, /* Body : 3 */
                    OPTIONAL_PARSER,      /* Else-Part : 4 */
                    GEN_STRING_R1_PARSER, /* Fi : 5 */
    )
    CSTNode else_part = *if_clause.children[4];
    size_t num_condition_executables = 0,
           num_body_executables = 0,
           num_else_executables = 0;
    Executable * condition_executables = imperfect_abstract_compound_list(*if_clause.children[1], &num_condition_executables);
    Executable * body_executables = imperfect_abstract_compound_list(*if_clause.children[3], &num_body_executables);
    Executable * else_executables = NULL;
    if (has_children(else_part)) {
        NODE_COMPLIANCE(else_part, OPTIONAL_PARSER, 1, ELSE_PART_PARSER)
        else_executables = imperfect_abstract_else_part(*else_part.children[0], &num_else_executables);
    }
    return (Executable) {
            .type = EXEC_IF,
            .executable = (union ExecutableUnion) {
                    .if_stat = (struct ExecIf) {
                            .num_condition_commands = num_condition_executables,
                            .condition_commands = condition_executables,
                            .num_body_commands = num_body_executables,
                            .body_commands = body_executables,
                            .num_else_commands = num_else_executables,
                            .else_commands = else_executables,
                    },
            },
    };
}

static Executable * imperfect_abstract_else(CSTNode else_clause, size_t * num_executables);
static Executable * imperfect_abstract_elif(CSTNode elif_clause, size_t * num_executables);

Executable * imperfect_abstract_else_part(CSTNode else_part, size_t * num_executables)
{
    PARENT_NODE_COMPLIANCE(else_part, ELSE_PART_PARSER, 1)
    *num_executables = 0;
    else_part = *else_part.children[0]; // to SEQUENCE_PARSER
    if (else_part.num_children == 5)
        return imperfect_abstract_elif(else_part, num_executables);
    assert(else_part.num_children == 2);
    return imperfect_abstract_else(else_part, num_executables);
}

static Executable * imperfect_abstract_else(CSTNode else_clause, size_t * num_executables)
{
    NODE_COMPLIANCE(else_clause, SEQUENCE_PARSER, 2, GEN_STRING_R1_PARSER, COMPOUND_LIST_PARSER)
    return imperfect_abstract_compound_list(*else_clause.children[1], num_executables);
}

static Executable * imperfect_abstract_elif(CSTNode elif_clause, size_t * num_executables)
{
    NODE_COMPLIANCE(elif_clause, SEQUENCE_PARSER, 5,
                    GEN_STRING_R1_PARSER, /* Elif : 0 */
                    COMPOUND_LIST_PARSER, /* Condition : 1 */
                    GEN_STRING_R1_PARSER, /* Then : 2 */
                    COMPOUND_LIST_PARSER, /* Body : 3 */
                    OPTIONAL_PARSER,      /* Else-Part : 4 */
    )
    *num_executables = 1;
    CSTNode else_part = *elif_clause.children[4];
    size_t num_condition_executables = 0,
            num_body_executables = 0,
            num_else_executables = 0;
    Executable * condition_executables = imperfect_abstract_compound_list(*elif_clause.children[1], &num_condition_executables);
    Executable * body_executables = imperfect_abstract_compound_list(*elif_clause.children[3], &num_body_executables);
    Executable * else_executables = NULL;
    if (has_children(else_part)) {
        NODE_COMPLIANCE(else_part, OPTIONAL_PARSER, 1, ELSE_PART_PARSER)
        else_executables = imperfect_abstract_else_part(*else_part.children[0], &num_else_executables);
    }
    Executable * executable_heap = reg_malloc(sizeof(Executable));
    *executable_heap = (Executable) {
            .type = EXEC_IF,
            .executable = (union ExecutableUnion) {
                    .if_stat = (struct ExecIf) {
                            .num_condition_commands = num_condition_executables,
                            .condition_commands = condition_executables,
                            .num_body_commands = num_body_executables,
                            .body_commands = body_executables,
                            .num_else_commands = num_else_executables,
                            .else_commands = else_executables,
                    },
            },
    };
    return executable_heap;
}
