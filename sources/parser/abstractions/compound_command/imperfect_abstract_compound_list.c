//
// Created by Nicolas Reyland on 8/29/22.
//

#include "parser/abstractions/compound_command/imperfect_abstract_compound_list.h"
#include "misc/output.h"
#include "parser/abstractions/abstraction.h"
#include "parser/abstractions/abstract_and_or.h"
#include "misc/safemem.h"

Executable * imperfect_abstract_compound_list(CSTNode compound_list, size_t * num_executables)
{
    NODE_COMPLIANCE(compound_list, COMPOUND_LIST_PARSER, 3, LINEBREAK_PARSER, TERM_PARSER, OPTIONAL_PARSER)
    CSTNode term = *compound_list.children[1];
    NODE_COMPLIANCE(term, TERM_PARSER, 2, AND_OR_PARSER, SEPARATED_REPETITION_PARSER)
    // First and-or
    *num_executables = 1 + term.children[1]->num_children;
    Executable * execs = reg_malloc(*num_executables * sizeof(Executable));
    execs[0] = abstract_and_or(*term.children[0]);
    // Rest of them
    for (size_t i = 0; i < term.children[1]->num_children; i++) {
        CSTNode child = *term.children[1]->children[i];
        NODE_COMPLIANCE(child, SEQUENCE_PARSER, 2, SEPARATOR_PARSER, AND_OR_PARSER)
        execs[i + 1] = abstract_and_or(*child.children[1]);
    }

    return execs;
}
