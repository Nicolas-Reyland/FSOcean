//
// Created by Nicolas Reyland on 8/29/22.
//

#include "parser/abstractions/compound_command/abstract_for_clause.h"
#include "parser/abstractions/abstraction.h"
#include "misc/safemem.h"
#include "parser/abstractions/compound_command/imperfect_abstract_compound_list.h"

Executable abstract_for_clause(CSTNode for_clause) {
    NODE_COMPLIANCE(for_clause, FOR_CLAUSE_PARSER, 4, GEN_STRING_R1_PARSER, NAME_PARSER, OPTIONAL_PARSER, DO_GROUP_PARSER)
    // Setup variables
    char ** wordlist = NULL;
    struct Executable * body_commands = NULL;
    size_t wordlist_len = 0,
           num_body_commands = 0;
    // Wordlist
    if (has_children(*for_clause.children[2])) {
        // There may be a wordlist
        CSTNode wordlist_node = *for_clause.children[2]; // to OPTIONAL_PARSER
        NODE_COMPLIANCE(wordlist_node, OPTIONAL_PARSER, 1, SEQUENCE_PARSER)
        wordlist_node = *wordlist_node.children[0]; // to SEQUENCE_PARSER
        NODE_COMPLIANCE(wordlist_node, SEQUENCE_PARSER, 2, OPTIONAL_PARSER, SEQUENTIAL_SEP_PARSER)
        wordlist_node = *wordlist_node.children[0]; // to OPTIONAL_PARSER
        if (has_children(wordlist_node)) {
            // There may be a wordlist (still not sure)
            NODE_COMPLIANCE(wordlist_node, OPTIONAL_PARSER, 1, SEQUENCE_PARSER)
            wordlist_node = *wordlist_node.children[0]; // to SEQUENCE_PARSER
            NODE_COMPLIANCE(wordlist_node, SEQUENCE_PARSER, 3, LINEBREAK_PARSER, IN_PARSER, OPTIONAL_PARSER)
            wordlist_node = *wordlist_node.children[2]; // to OPTIONAL_PARSER
            if (has_children(wordlist_node)) {
                // There IS a wordlist !
                NODE_COMPLIANCE(wordlist_node, OPTIONAL_PARSER, 1, WORDLIST_PARSER)
                wordlist_node = *wordlist_node.children[0]; // to WORDLIST_PARSER
                NODE_COMPLIANCE(wordlist_node, WORDLIST_PARSER, 2, TK_WORD_PARSER, REPETITION_PARSER)
                CSTNode wordlist_node_repetition = *wordlist_node.children[1];
                wordlist_len = 1 + wordlist_node_repetition.num_children;
                // Copy words
                wordlist = reg_calloc(wordlist_len, sizeof(char *));
                wordlist[0] = wordlist_node.children[0]->token->str;
                for (size_t i = 0; i < wordlist_node_repetition.num_children; i++) {
                    wordlist[i + 1] = wordlist_node_repetition.children[i]->token->str;
                }
            }
        }
    }
    // Do Group
    CSTNode do_group = *for_clause.children[3];
    NODE_COMPLIANCE(do_group, DO_GROUP_PARSER, 3, GEN_STRING_R1_PARSER, COMPOUND_LIST_PARSER, GEN_STRING_R1_PARSER)
    body_commands = imperfect_abstract_compound_list(*do_group.children[1], &num_body_commands);
    return (Executable) {
        .type = EXEC_FOR_LOOP,
        .executable = (union ExecutableUnion) {
            .for_loop = (struct ExecForLoop) {
                    .var_name = for_clause.children[1]->token->str,
                    .var_name_len = for_clause.children[1]->token->str_len,
                    .wordlist = wordlist,
                    .wordlist_len = wordlist_len,
                    .body_executables = body_commands,
                    .num_body_executables = num_body_commands,
            },
        },
    };
}
