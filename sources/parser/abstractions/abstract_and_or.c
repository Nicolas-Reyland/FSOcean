//
// Created by Nicolas Reyland on 8/27/22.
//

#include <string.h>
#include "parser/abstractions/abstract_and_or.h"
#include "parser/abstractions/abstraction.h"
#include "parser/abstractions/abstract_pipeline.h"
#include "misc/output.h"
#include "misc/safemem.h"

Executable abstract_and_or(CSTNode cst_node) {
    NODE_COMPLIANCE(cst_node, AND_OR_PARSER, 2, GENERATOR_PARSER, SEPARATED_REPETITION_PARSER)
    CSTNode first_pipeline = *cst_node.children[0],
            repetition = *cst_node.children[1];
    Executable first_pipeline_executable = abstract_pipeline(first_pipeline);
    if (!has_children(repetition)) {
        // Single pipeline
        return first_pipeline_executable;
    }
    // Use of && or ||
    struct ExecMultiExecutables multi_or = {
            .execution_flags = EXE_OR_FLAG,
            .num_executables = 0,
            .executables = NULL,
    };
    /*
     * A && B && C || D || E && F -> (A && B && C) || D || (E && F)
     */
    size_t and_commands_buffer_size = 1;
    Executable * and_commands_buffer = reg_malloc(sizeof(Executable));
    and_commands_buffer[0] = first_pipeline_executable;
    for (size_t i = 0; i < repetition.num_children; i++) {
        CSTNode sequence = *repetition.children[i];
        NODE_COMPLIANCE(sequence, SEQUENCE_PARSER, 2, SEQUENCE_PARSER, GENERATOR_PARSER)
        CSTNode seq_logic_op = *sequence.children[0]; // to SEQUENCE_PARSER
        NODE_COMPLIANCE(seq_logic_op, SEQUENCE_PARSER, 2, CHOICE_PARSER, LINEBREAK_PARSER)
        seq_logic_op = *seq_logic_op.children[0]; // to CHOICE_PARSER
        NODE_COMPLIANCE(seq_logic_op, CHOICE_PARSER, 1, GEN_STRING_PARSER)
        seq_logic_op = *seq_logic_op.children[0]; // to GEN_STRING_PARSER
        CSTNode seq_pipeline = *sequence.children[1]; // to GENERATOR_PARSER (is verified later)
        if (strcmp(seq_logic_op.token->str, "&&") == 0) {
            // AND operator
            and_commands_buffer_size++;
            and_commands_buffer = reg_realloc(and_commands_buffer, and_commands_buffer_size * sizeof(Executable));
        } else if (strcmp(seq_logic_op.token->str, "||") == 0) {
            // OR operator
            multi_or.num_executables++;
            multi_or.executables = reg_realloc(multi_or.executables, multi_or.num_executables * sizeof(Executable));
            multi_or.executables[multi_or.num_executables - 1] = (Executable) {
                    .type = EXEC_MULTI,
                    .executable = (union ExecutableUnion) {
                        .multi = (struct ExecMultiExecutables) {
                            .execution_flags = EXE_AND_FLAG,
                            .num_executables = and_commands_buffer_size,
                            .executables = and_commands_buffer,
                        },
                    },
            };
            // Reset AND buffer
            and_commands_buffer = reg_malloc(sizeof(Executable));
            and_commands_buffer_size = 1;
        } else
            print_error(OCERR_EXIT, "Unexpected logic-operator string: '%s'\n", seq_logic_op.token->str);
        // Add last sequence unit to the AND buffer
        and_commands_buffer[and_commands_buffer_size - 1] = abstract_pipeline(seq_pipeline);
    }
    if (and_commands_buffer_size != 0) {
        multi_or.num_executables++;
        multi_or.executables = reg_realloc(multi_or.executables, multi_or.num_executables * sizeof(Executable));
        multi_or.executables[multi_or.num_executables - 1] = (Executable) {
                .type = EXEC_MULTI,
                .executable = (union ExecutableUnion) {
                        .multi = (struct ExecMultiExecutables) {
                                .execution_flags = EXE_AND_FLAG,
                                .num_executables = and_commands_buffer_size,
                                .executables = and_commands_buffer,
                        },
                },
        };
    }
    return (Executable) {
        .type = EXEC_MULTI,
        .executable = {
                .multi = multi_or,
        },
    };
}
