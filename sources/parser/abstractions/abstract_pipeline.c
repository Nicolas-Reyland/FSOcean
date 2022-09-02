//
// Created by Nicolas Reyland on 8/27/22.
//

#include <stdlib.h>
#include "parser/abstractions/abstract_pipeline.h"
#include "parser/abstractions/abstraction.h"
#include "misc/output/output.h"
#include "parser/abstractions/abstract_compound_command.h"
#include "parser/abstractions/imperfect_abstract_io_redirect.h"
#include "misc/safemem.h"
#include "parser/abstractions/imperfect_abstract_redirect_list.h"

static Executable abstract_command(CSTNode cst_node);
static Executable abstract_simple_command(CSTNode cst_node);
static Executable abstract_function_definition(CSTNode function_definition);

Executable abstract_pipeline(CSTNode cst_node) {
    NODE_COMPLIANCE(cst_node, PIPELINE_PARSER, 2, OPTIONAL_PARSER, PIPE_SEQUENCE_PARSER)
    // TODO: Bang symbol support
    cst_node = *cst_node.children[1]; // to PIPE_SEQUENCE
    NODE_COMPLIANCE(cst_node, PIPE_SEQUENCE_PARSER, 2, COMMAND_PARSER, SEPARATED_REPETITION_PARSER)
    Executable first_command = abstract_command(*cst_node.children[0]);
    if (has_children(*cst_node.children[1])) {
        // Multi commands which use pipes (at least two)
        size_t num_children = cst_node.children[1]->num_children + 1; // + first command
        struct ExecMultiExecutables pipe_sequence = {
                .execution_flags = EXE_PIPE_FLAG,
                .executables = reg_calloc(num_children, sizeof(struct Executable)),
                .num_executables = num_children,
        };
        pipe_sequence.executables[0] = first_command;
        for (size_t i = 0; i < cst_node.children[1]->num_children; i++)
            pipe_sequence.executables[i + 1] = abstract_command(*cst_node.children[1]->children[i]);
        return (Executable) {
            .type = EXEC_MULTI,
            .executable = (union ExecutableUnion) {
                .multi = pipe_sequence,
            },
        };
    }
    // Single command
    return first_command;
}

static Executable abstract_command(CSTNode cst_node) {
    PARENT_NODE_COMPLIANCE(cst_node, COMMAND_PARSER, 1)
    cst_node = *cst_node.children[0]; // to 
    // 3 possibilities :
    switch (cst_node.type) {
        case SIMPLE_COMMAND_PARSER: // 1. Simple command
            NODE_COMPLIANCE(cst_node, SIMPLE_COMMAND_PARSER, 1, SEQUENCE_PARSER)
            cst_node = *cst_node.children[0];
            return abstract_simple_command(cst_node);
        case SEQUENCE_PARSER: // 2. Compound command
            NODE_COMPLIANCE(cst_node, SEQUENCE_PARSER, 2, COMPOUND_COMMAND_PARSER, OPTIONAL_PARSER)
            Executable command = abstract_compound_command(*cst_node.children[0]);
            // No redirect
            if (!has_children(*cst_node.children[1]))
                return command;
            // There are redirects
            Executable * command_heap = reg_malloc(sizeof(Executable));
            *command_heap = command;
            unsigned long * flags = NULL;
            char ** files = NULL;
            size_t num_redirect = imperfect_abstract_redirect_list(*cst_node.children[1]->children[0], &flags, &files);
            return (Executable) {
                    .type = EXEC_REDIRECT,
                    .executable = (union ExecutableUnion) {
                            .redirect = (struct ExecRedirect) {
                                    .num_redirects = num_redirect,
                                    .flags = flags,
                                    .files = files,
                                    .executable = command_heap,
                            },
                    },
            };
        case FUNCTION_DEFINITION_PARSER: // 3. Function definition
            return abstract_function_definition(cst_node);
        default:
            print_error(OCERR_EXIT, "Command child has type %d\n", cst_node.type);
            exit(1);
    }
}

static void imperfect_abstract_ffix(CSTNode ffix_node, struct ExecRedirect * redirects, struct ExecCommandWord ** words, enum ExecCommandWordType word_type, size_t * num_words);

static Executable abstract_simple_command(CSTNode cst_node)
{
    PARENT_NODE_COMPLIANCE(cst_node, SEQUENCE_PARSER, 2);
    assert(cst_node.children[1]->type == OPTIONAL_PARSER); // so that we just need to check for the first child's type
    // Declare three major nodes
    CSTNode * prefix_node = NULL;
    CSTNode * word_node = NULL;
    CSTNode * suffix_node = NULL;
    CSTNode optional_node = *cst_node.children[1];
    // Prefix-type thing
    if (cst_node.children[0]->type == CMD_PREFIX_PARSER) {
        prefix_node = cst_node.children[0];
        NODE_COMPLIANCE((*prefix_node), CMD_PREFIX_PARSER, 2, CHOICE_PARSER, REPETITION_PARSER)
        if (has_children(optional_node)) {
            NODE_COMPLIANCE(optional_node, OPTIONAL_PARSER, 1, SEQUENCE_PARSER)
            CSTNode sequence = *optional_node.children[0]; // SEQUENCE_PARSER
            NODE_COMPLIANCE(sequence, SEQUENCE_PARSER, 2, CMD_WORD_PARSER, OPTIONAL_PARSER)
            word_node = sequence.children[0];
            NODE_COMPLIANCE((*word_node), CMD_WORD_PARSER, 0)
            word_node->type = CMD_NAME_PARSER; // changing type of parser, for later compatibility (should not matter)
            // suffix node, if exists
            if (has_children(*sequence.children[1])) {
                NODE_COMPLIANCE((*sequence.children[1]), OPTIONAL_PARSER, 1, CMD_SUFFIX_PARSER)
                suffix_node = sequence.children[1]->children[0];
                NODE_COMPLIANCE((*suffix_node), CMD_SUFFIX_PARSER, 2, CHOICE_PARSER, REPETITION_PARSER)
            }
        }
    }
    // No-prefix-type thing
    else {
        assert(cst_node.children[0]->type == CMD_NAME_PARSER);
        word_node = cst_node.children[0];
        NODE_COMPLIANCE((*word_node), CMD_NAME_PARSER, 0)
        if (has_children(optional_node)) {
            NODE_COMPLIANCE(optional_node, OPTIONAL_PARSER, 1, CMD_SUFFIX_PARSER)
            suffix_node = optional_node.children[0];
            NODE_COMPLIANCE((*suffix_node), CMD_SUFFIX_PARSER, 2, CHOICE_PARSER, REPETITION_PARSER)
        }
    }
    // Start with empty/null variables
    struct ExecCommandWord * words = NULL;
    size_t num_words = 0;
    struct ExecRedirect redirects = {
            .executable = NULL,
            .flags = NULL,
            .files = NULL,
            .num_redirects = 0,
    };
    // add assignment words & redirects (if any)
    if (prefix_node != NULL)
        imperfect_abstract_ffix(*prefix_node, &redirects, &words, ECW_ASSIGNMENT, &num_words);
    // add first word (if any)
    if (word_node != NULL) {
        num_words++;
        words = reg_realloc(words, num_words * sizeof(struct ExecCommandWord));
        words[num_words - 1] = create_exec_command_word(ECW_WORD, word_node->token->str, word_node->token->str_len);
    }
    // add command words & add redirects (if any)
    if (suffix_node != NULL)
        imperfect_abstract_ffix(*suffix_node, &redirects, &words, ECW_WORD, &num_words);
    // create simple command using all the words
    Executable simple_command = create_exec_command(words, num_words);
    // no redirects
    if (redirects.num_redirects == 0)
        return simple_command;
    // there is at least one redirect
    Executable * simple_command_heap = reg_malloc(sizeof(Executable));
    *simple_command_heap = simple_command;
    redirects.executable = simple_command_heap;
    return (Executable) {
            .type = EXEC_REDIRECT,
            .executable = (union ExecutableUnion) {
                    .redirect = redirects,
            },
    };
}

static void commit_unidentified_node(CSTNode cst_node, struct ExecRedirect * redirects, struct ExecCommandWord ** words, enum ExecCommandWordType word_type, size_t * num_words);

/*
 * Same thing for cmd-prefix- and cmd-suffix-nodes :
 * extract redirects and the other type of node from this kind of structure
 */
static void imperfect_abstract_ffix(CSTNode ffix_node, struct ExecRedirect * redirects, struct ExecCommandWord ** words, enum ExecCommandWordType word_type, size_t * num_words)
{
    // no need for node compliance : has already been checked
    CSTNode first_node = *ffix_node.children[0], // CHOICE_PARSER
            repetition = *ffix_node.children[1]; // REPETITION_PARSER
    PARENT_NODE_COMPLIANCE(first_node, CHOICE_PARSER, 1)
    first_node = *first_node.children[0]; // to ? or IO_REDIRECT_PARSER
    commit_unidentified_node(first_node, redirects, words, word_type, num_words);
    for (size_t i = 0; i < repetition.num_children; i++) {
        CSTNode choice_node = *repetition.children[i];
        PARENT_NODE_COMPLIANCE(choice_node, CHOICE_PARSER, 1)
        commit_unidentified_node(*choice_node.children[0], redirects, words, word_type, num_words);
    }
}

static void commit_unidentified_node(CSTNode cst_node, struct ExecRedirect * redirects, struct ExecCommandWord ** words, enum ExecCommandWordType word_type, size_t * num_words)
{
    if (cst_node.type == IO_REDIRECT_PARSER) {
        size_t num_redirects = ++redirects->num_redirects;
        redirects->flags = reg_realloc(redirects->flags, num_redirects * sizeof(unsigned long));
        redirects->files = reg_realloc(redirects->files, num_redirects * sizeof(char *));
        imperfect_abstract_io_redirect(cst_node, &(redirects->flags[num_redirects - 1]), &(redirects->files[num_redirects - 1]));
        return;
    }
    // not an io_redirect, so the unidentified type
    // there is a word-like thing, so we have to allocate more space in 'words'
    (*num_words)++;
    *words = reg_realloc(*words, (*num_words) * sizeof(struct ExecCommandWord));
    (*words)[*num_words - 1] = create_exec_command_word(word_type, cst_node.token->str, cst_node.token->str_len);
}

static Executable abstract_function_definition(CSTNode function_definition) {
    NOT_IMPLEMENTED_ERROR(proper abstraction of function definition)
    NODE_COMPLIANCE(function_definition, FUNCTION_DEFINITION_PARSER, 5,
                    FNAME_PARSER,
                    GEN_STRING_PARSER,
                    GEN_STRING_PARSER,
                    LINEBREAK_PARSER,
                    FUNCTION_BODY_PARSER
    )
    CSTNode function_body = *function_definition.children[4]; // FUNCTION_BODY_PARSER
    NODE_COMPLIANCE(function_body, FUNCTION_BODY_PARSER, 2, COMPOUND_COMMAND_PARSER, OPTIONAL_PARSER)
    CSTNode compound_command_node = *function_body.children[0],
            redirect_list_node = *function_body.children[1];
    Executable compound_command = abstract_compound_command(compound_command_node);
    if (!has_children(redirect_list_node)) // no redirection
        return compound_command;
    // at least one redirection
    NODE_COMPLIANCE(redirect_list_node, OPTIONAL_PARSER, 1, REDIRECT_LIST_PARSER)
    Executable * compound_command_heap = reg_malloc(sizeof(Executable));
    *compound_command_heap = compound_command;
    redirect_list_node = *redirect_list_node.children[0];
    // create redirection
    unsigned long * flags = NULL;
    char ** files = NULL;
    size_t num_redirect = imperfect_abstract_redirect_list(redirect_list_node, &flags, &files);
    return (Executable) {
            .type = EXEC_REDIRECT,
            .executable = (union ExecutableUnion) {
                    .redirect = (struct ExecRedirect) {
                            .num_redirects = num_redirect,
                            .flags = flags,
                            .files = files,
                            .executable = compound_command_heap,
                    },
            },
    };
}
