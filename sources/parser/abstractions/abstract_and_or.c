//
// Created by Nicolas Reyland on 8/27/22.
//

#include "parser/abstractions/abstract_and_or.h"
#include "parser/abstractions/abstraction.h"
#include "parser/abstractions/abstract_pipeline.h"
#include "misc/output.h"

Executable abstract_and_or(CSTNode cst_node) {
    NODE_COMPLIANCE(cst_node, AND_OR_PARSER, 2, GENERATOR_PARSER, SEPARATED_REPETITION_PARSER)
    if (!has_children(cst_node)) {
        return abstract_pipeline(cst_node);
    }
    NOT_IMPLEMENTED_ERROR(abstract and-or with multiple pipelines)
}
