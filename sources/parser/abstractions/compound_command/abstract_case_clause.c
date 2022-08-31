//
// Created by Nicolas Reyland on 8/29/22.
//

#include "parser/abstractions/compound_command/abstract_case_clause.h"
#include "parser/abstractions/abstraction.h"
#include "misc/safemem.h"
#include "parser/abstractions/compound_command/imperfect_abstract_compound_list.h"

static Executable abstract_case_item(CSTNode case_item_ns);
static Executable abstract_case_item_ns(CSTNode case_item_ns);

Executable abstract_case_clause(CSTNode case_clause) {
    NODE_COMPLIANCE(case_clause, CASE_CLAUSE_PARSER, 7,
                    GEN_STRING_R1_PARSER,   /* case : 0 */
                    TK_WORD_PARSER,         /* word : 1 */
                    LINEBREAK_PARSER,       /* : 2 */
                    IN_PARSER,              /* in : 3 */
                    LINEBREAK_PARSER,       /* : 4 */
                    OPTIONAL_PARSER,        /* body : 5 */
                    GEN_STRING_R1_PARSER,   /* esac : 6 */
    )
    char * word = case_clause.children[1]->token->str;
    size_t word_len = case_clause.children[1]->token->str_len;
    CSTNode case_items = *case_clause.children[5];
    if (!has_children(case_items)) // no patterns, no nothing
        return (Executable) {
                .type = EXEC_CASE,
                .executable = (union ExecutableUnion) {
                        .case_clause = (struct ExecCase) {
                                .word = word,
                                .word_len = word_len,
                                .num_patterns = 0,
                                .cases = NULL,
                        },
                },
        };
    // There is at least one pattern
    NODE_COMPLIANCE(case_items, OPTIONAL_PARSER, 1, SEQUENCE_PARSER)
    case_items = *case_items.children[0]; // to SEQUENCE_PARSER
    NODE_COMPLIANCE(case_items, SEQUENCE_PARSER, 2, REPETITION_PARSER, OPTIONAL_PARSER)
    // Setup variables & memory
    CSTNode repetition = *case_items.children[0],
            optional = *case_items.children[1];
    size_t num_patterns = repetition.num_children + optional.num_children;
    Executable * cases = reg_calloc(num_patterns, sizeof(Executable));
    // Fill the case-items from the repetition
    for (size_t i = 0; i < repetition.num_children; i++)
        cases[i] = abstract_case_item(*repetition.children[i]);
    // Add the case-item-ns if it exists
    if (has_children(optional))
        cases[repetition.num_children] = abstract_case_item_ns(*optional.children[0]);
    return (Executable) {
            .type = EXEC_CASE,
            .executable = (union ExecutableUnion) {
                    .case_clause = (struct ExecCase) {
                            .word = word,
                            .word_len = word_len,
                            .num_patterns = num_patterns,
                            .cases = cases,
                    },
            },
    };
}

static Executable imperfect_abstract_pattern(CSTNode pattern);

Executable abstract_case_item(CSTNode case_item) {
    NODE_COMPLIANCE(case_item, CASE_ITEM_PARSER, 3, CASE_ITEM_NS_PARSER, GEN_STRING_PARSER, LINEBREAK_PARSER)
    return abstract_case_item_ns(*case_item.children[0]);
}

Executable abstract_case_item_ns(CSTNode case_item_ns) {
    NODE_COMPLIANCE(case_item_ns, CASE_ITEM_NS_PARSER, 4,
                    OPTIONAL_PARSER,
                    PATTERN_PARSER,
                    GEN_STRING_PARSER,
                    CHOICE_PARSER,
    )
    struct CSTNode compound_list = *case_item_ns.children[3]; // to CHOICE_PARSER
    PARENT_NODE_COMPLIANCE(compound_list, CHOICE_PARSER, 1)
    compound_list = *compound_list.children[0]; // to COMPOUND_LIST_PARSER || LINEBREAK_PARSER
    if (compound_list.type == LINEBREAK_PARSER) // empty case clause (not bothering to extract the patterns)
        return create_exec_multi_executables(
                EXE_CASE_ITEM,
                NULL,
                0
        );
    // There is a body to the case clause
    Executable pattern = imperfect_abstract_pattern(*case_item_ns.children[1]);
    size_t num_executables = 0;
    Executable * executables = imperfect_abstract_compound_list(compound_list, &num_executables);
    executables = reg_realloc(executables, (num_executables + 1) * sizeof(Executable));
    // the last executable is the pattern executable
    executables[num_executables] = pattern;
    return create_exec_multi_executables(
            EXE_CASE_ITEM,
            executables,
            num_executables + 1
    );
}

Executable imperfect_abstract_pattern(CSTNode pattern) {
    assert(pattern.type == PATTERN_PARSER);
    if (pattern.num_children == 1) {
        NODE_COMPLIANCE(pattern, PATTERN_PARSER, 1, SEQUENCE_PARSER)
        pattern = *pattern.children[0]; // to SEQUENCE_PARSER
        NODE_COMPLIANCE(pattern, SEQUENCE_PARSER, 2, TK_WORD_PARSER, INVERTED_PARSER)
        pattern = *pattern.children[0]; // to TK_WORD_PARSER
        struct ExecCommandWord * pattern_word = reg_malloc(sizeof(struct ExecCommandWord));
        *pattern_word = create_exec_command_word(ECW_PATTERN, pattern.token->str, pattern.token->str_len);
        return create_exec_command(pattern_word, 1); // this is a fake command, just used to store the patterns
    }
    // Multiple patterns
    NODE_COMPLIANCE(pattern, SEPARATED_REPETITION_PARSER, 2, TK_WORD_PARSER, GEN_STRING_PARSER)
    // TODO: test this part
    CSTNode first_pattern = *pattern.children[0],
            rest_patterns = *pattern.children[1];
    size_t num_pattern_words = 1 + rest_patterns.num_children;
    struct ExecCommandWord * pattern_words = reg_calloc(num_pattern_words, sizeof(struct ExecCommandWord));
    pattern_words[0] = create_exec_command_word(ECW_PATTERN, first_pattern.token->str, first_pattern.token->str_len);
    for (size_t i = 0; i < rest_patterns.num_children; i++)
        pattern_words[i + 1] = create_exec_command_word(ECW_PATTERN, rest_patterns.children[i]->token->str, rest_patterns.children[i]->token->str_len);
    return create_exec_command(pattern_words, num_pattern_words);
}
