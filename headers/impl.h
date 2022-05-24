//
// Created on 17/05/2022.
//

#include "combinators/combinator.h"
#include "parser/parse_context.h"

#ifndef OCEAN_IMPL_H
#define OCEAN_IMPL_H

enum ParserType {
    // Grammar Parsers
    PROGRAM_PARSER,
    COMPLETE_COMMANDS_PARSER,
    COMPLETE_COMMAND_PARSER,
    LIST_PARSER,
    AND_OR_PARSER,
    PIPELINE_PARSER,
    PIPE_SEQUENCE_PARSER,
    COMMAND_PARSER,
    COMPOUND_COMMAND_PARSER,
    SUBSHELL_PARSER,
    COMPOUND_LIST_PARSER,
    TERM_PARSER,
    FOR_CLAUSE_PARSER,
    NAME_PARSER,
    IN_PARSER,
    WORDLIST_PARSER,
    CASE_CLAUSE_PARSER,
    CASE_LIST_NS_PARSER,
    CASE_LIST_PARSER,
    CASE_ITEM_NS_PARSER,
    CASE_ITEM_PARSER,
    PATTERN_PARSER,
    IF_CLAUSE_PARSER,
    ELSE_PART_PARSER,
    WHILE_CLAUSE_PARSER,
    UNTIL_CLAUSE_PARSER,
    FUNCTION_DEFINITION_PARSER,
    FUNCTION_BODY_PARSER,
    FNAME_PARSER,
    BRACE_GROUP_PARSER,
    DO_GROUP_PARSER,
    SIMPLE_COMMAND_PARSER,
    CMD_NAME_PARSER,
    CMD_WORD_PARSER,
    CMD_PREFIX_PARSER,
    CMD_SUFFIX_PARSER,
    REDIRECT_LIST_PARSER,
    IO_REDIRECT_PARSER,
    IO_FILE_PARSER,
    FILENAME_PARSER,
    IO_HERE_PARSER,
    HERE_END_PARSER,
    NEWLINE_LIST_PARSER,
    LINEBREAK_PARSER,
    SEPARATOR_OP_PARSER,
    SEPARATOR_PARSER,
    SEQUENTIAL_SEP_PARSER,
    // Token Parsers
    TK_WORD_PARSER,
    TK_ASSIGNMENT_WORD_PARSE,
    TK_NAME_PARSER,
    TK_NEWLINE_PARSER,
    TK_IO_NUMBER,
    // Abstract Combinators
    SEQUENCE_PARSER = COMBINATOR_SEQUENCE_TYPE,
    SEQUENCE_PARSER_UNIT = COMBINATOR_SEQUENCE_UNIT_TYPE,
    CHOICE_PARSER = COMBINATOR_CHOICE_TYPE,
    OPTIONAL_PARSER = COMBINATOR_OPTIONAL_TYPE,
};

Combinator program_parser();

#endif // OCEAN_IMPL_H
