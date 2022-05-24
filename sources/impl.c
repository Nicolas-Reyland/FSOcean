//
// Created on 17/05/2022.
//

#include <string.h>
#include "impl.h"
#include "parser/parser.h"

// Utils
#define STRING_PARSER(name, match) \
static bool _gen_string_parser_##name##_parse_f(void * void_ctx, Combinator * p) \
{ \
    (void)p; \
    ParseContext * ctx = void_ctx; \
    Token token = ctx->tokens[ctx->pos++]; \
    return strcmp(token.str, match) == 0; \
} \
static Combinator name##_parser() \
{ \
    return typed_cmb(PARSER_CREATE(_gen_string_parser_##name##_parse_f, parser_commit_single_token), CST_STRING_PARSER); \
} \


/* OFFICIAL GRAMMAR IMPLEMENTATION */

/* -------------------------------------------------------
   The grammar symbols
   ------------------------------------------------------- */
static Combinator WORD_parser(void);
static Combinator ASSIGNMENT_WORD_parser(void);
static Combinator NAME_parser(void);
static Combinator NEWLINE_parser(void);
static Combinator IO_NUMBER_parser(void);

/* The following are the operators (see XBD Operator)
    containing more than one character. */
STRING_PARSER(AND_IF, "&&")
STRING_PARSER(OR_IF, "||")
STRING_PARSER(DSEMI, ";;")

STRING_PARSER(DLESS, "<<")
STRING_PARSER(DGREAT, ">>")
STRING_PARSER(LESSAND, "<&")
STRING_PARSER(GREATAND, ">&")
STRING_PARSER(LESSGREAT, "<>")
STRING_PARSER(DLESSDASH, "<<-")

STRING_PARSER(CLOBBER, ">|")

/* The following are the reserved words. */
STRING_PARSER(If, "if")
STRING_PARSER(Then, "then")
STRING_PARSER(Else, "else")
STRING_PARSER(Elif, "elif")
STRING_PARSER(Fi, "fi")
STRING_PARSER(Do, "do")
STRING_PARSER(Done, "done")

STRING_PARSER(Case, "case")
STRING_PARSER(Esac, "esac")
STRING_PARSER(While, "while")
STRING_PARSER(Until, "until")
STRING_PARSER(For, "for")

/* These are reserved words, not operator tokens, and are
    recognized when reserved words are recognized. */
STRING_PARSER(Lbrace, "{")
STRING_PARSER(Rbrace, "}")
STRING_PARSER(Bang, "!")

STRING_PARSER(In, "in")

/* Some more parsers for simple string/char parsing */
STRING_PARSER(sub_Opening_Parenthesis, "(")
STRING_PARSER(sub_Pipe, "|")
STRING_PARSER(sub_Closing_Parenthesis, ")")
STRING_PARSER(sub_Less, "<")
STRING_PARSER(sub_Greater, ">")
STRING_PARSER(sub_Ampersand, "&")
STRING_PARSER(sub_Semicolon, ";")


/* -------------------------------------------------------
   The Grammar
   ------------------------------------------------------- */

static Combinator complete_commands_parser(void);
static Combinator complete_command_parser(void);
static Combinator list_parser(void);
static Combinator and_or_parser(void);
static Combinator pipeline_parser(void);
static Combinator pipe_sequence_parser(void);
static Combinator command_parser(void);
static Combinator compound_command_parser(void);
static Combinator subshell_parser(void);
static Combinator compound_list_parser(void);
static Combinator term_parser(void);
static Combinator for_clause_parser(void);
static Combinator name_parser(void);
static Combinator in_parser(void);
static Combinator wordlist_parser(void);
static Combinator case_clause_parser(void);
static Combinator case_list_ns_parser(void);
static Combinator case_list_parser(void);
static Combinator case_item_ns_parser(void);
static Combinator case_item_parser(void);
static Combinator pattern_parser(void);
static Combinator if_clause_parser(void);
static Combinator else_part_parser(void);
static Combinator while_clause_parser(void);
static Combinator until_clause_parser(void);
static Combinator function_definition_parser(void);
static Combinator function_body_parser(void);
static Combinator fname_parser(void);
static Combinator brace_group_parser(void);
static Combinator do_group_parser(void);
static Combinator simple_command_parser(void);
static Combinator cmd_name_parser(void);
static Combinator cmd_word_parser(void);
static Combinator cmd_prefix_parser(void);
static Combinator cmd_suffix_parser(void);
static Combinator redirect_list_parser(void);
static Combinator io_redirect_parser(void);
static Combinator io_file_parser(void);
static Combinator filename_parser(void);
static Combinator io_here_parser(void);
static Combinator here_end_parser(void);
static Combinator newline_list_parser(void);
static Combinator linebreak_parser(void);
static Combinator separator_op_parser(void);
static Combinator separator_parser(void);
static Combinator sequential_sep_parser(void);

/*
program          : linebreak complete_commands linebreak
                 | linebreak
                  ;
 */
Combinator program_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(2,
                    PARSER_CMB_SEQUENCE(3,
                            linebreak_parser(),
                            complete_commands_parser(),
                            linebreak_parser()
                    ),
                    linebreak_parser()
            ),
            PROGRAM_PARSER);
}

/*
complete_commands: complete_commands newline_list complete_command
                 |                                complete_command
                  ;
*/
static Combinator complete_commands_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(2,
                        PARSER_CMB_SEQUENCE(3,
                                PARSER_CMB_FORWARD_REF(complete_commands_parser),
                                newline_list_parser(),
                                complete_command_parser()
                        ),
                        complete_command_parser()
            ),
            COMPLETE_COMMANDS_PARSER);
}

/*
complete_command : list separator_op
                 | list
                  ;
*/
static Combinator complete_command_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(2,
                        list_parser(),
                        separator_op_parser()
            ),
            COMPLETE_COMMAND_PARSER);
}

/*
list             : list separator_op and_or
                 |                   and_or
                  ;
*/
static Combinator list_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(2,
                        PARSER_CMB_FORWARD_REF(list_parser),
                        separator_op_parser(),
                        and_or_parser()
            ),
            LIST_PARSER);
}

/*
and_or           :                         pipeline
                 | and_or AND_IF linebreak pipeline
                 | and_or OR_IF  linebreak pipeline
                  ;
*/
static Combinator and_or_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(
                        3,
                        pipeline_parser(),
                        PARSER_CMB_SEQUENCE(4,
                                PARSER_CMB_FORWARD_REF(and_or_parser),
                                AND_IF_parser(),
                                linebreak_parser(),
                                pipeline_parser()
                        ),
                        PARSER_CMB_SEQUENCE(4,
                                PARSER_CMB_FORWARD_REF(and_or_parser),
                                linebreak_parser(),
                                pipeline_parser()
                        )
            ),
            AND_OR_PARSER);
}

/*
pipeline         :      pipe_sequence
                 | Bang pipe_sequence
                  ;
*/
static Combinator pipeline_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(2,
                        pipe_sequence_parser(),
                        PARSER_CMB_SEQUENCE(2,
                                Bang_parser(),
                                pipe_sequence_parser()
                                )
            ),
            PIPELINE_PARSER);
}

/*
pipe_sequence    :                             command
                 | pipe_sequence '|' linebreak command
                  ;
*/
static Combinator pipe_sequence_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(3,
                              PARSER_CMB_FORWARD_REF(command_parser),
                              PARSER_CMB_SEQUENCE(
                                      4,
                                      PARSER_CMB_FORWARD_REF(pipe_sequence_parser),
                                      sub_Pipe_parser(),
                                      linebreak_parser(),
                                      PARSER_CMB_FORWARD_REF(command_parser)
                              )
            ),
            PIPE_SEQUENCE_PARSER);
}

/*
command          : simple_command
                 | compound_command
                 | compound_command redirect_list
                 | function_definition
                  ;
*/
static Combinator command_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(4,
                              simple_command_parser(),
                              compound_command_parser(),
                              PARSER_CMB_SEQUENCE(2,
                                      compound_command_parser(),
                                      redirect_list_parser()
                              ),
                              function_definition_parser()
            ),
            COMMAND_PARSER);
}

/*
compound_command : brace_group
                 | subshell
                 | for_clause
                 | case_clause
                 | if_clause
                 | while_clause
                 | until_clause
                  ;
*/
static Combinator compound_command_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(7,
                              brace_group_parser(),
                              subshell_parser(),
                              for_clause_parser(),
                              case_clause_parser(),
                              if_clause_parser(),
                              while_clause_parser(),
                              until_clause_parser()
            ),
            COMPOUND_COMMAND_PARSER);
}

/*
subshell         : '(' compound_list ')'
                  ;
*/
static Combinator subshell_parser()
{
    return typed_cmb(
            PARSER_CMB_SEQUENCE(3,
                              sub_Opening_Parenthesis_parser(),
                              compound_list_parser(),
                              sub_Closing_Parenthesis_parser()
            ),
            SUBSHELL_PARSER);
}

/*
compound_list    : linebreak term
                 | linebreak term separator
                  ;
*/
static Combinator compound_list_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(3,
                              PARSER_CMB_SEQUENCE(2,
                                      linebreak_parser(),
                                      term_parser()
                              ),
                              PARSER_CMB_SEQUENCE(3,
                                                  linebreak_parser(),
                                                  term_parser(),
                                                  separator_parser()
                              )
            ),
            COMPOUND_LIST_PARSER);
}

/*
term             : term separator and_or
                 |                and_or
                  ;
*/
static Combinator term_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(3,
                              PARSER_CMB_SEQUENCE(
                                      3,
                                      PARSER_CMB_FORWARD_REF(term_parser),
                                      separator_parser(),
                                      and_or_parser()
                              ),
                              and_or_parser()
            ),
            TERM_PARSER);
}

/*
for_clause       : For name                                      do_group
                 | For name                       sequential_sep do_group
                 | For name linebreak in          sequential_sep do_group
                 | For name linebreak in wordlist sequential_sep do_group
                  ;
*/
static Combinator for_clause_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(4,
                              PARSER_CMB_SEQUENCE(3,
                                      For_parser(),
                                      name_parser(),
                                      do_group_parser()
                              ),
                              PARSER_CMB_SEQUENCE(7,
                                                  For_parser(),
                                                  name_parser(),
                                                  sequential_sep_parser(),
                                                  do_group_parser()
                              ),
                              PARSER_CMB_SEQUENCE(7,
                                                  For_parser(),
                                                  name_parser(),
                                                  linebreak_parser(),
                                                  in_parser(),
                                                  sequential_sep_parser(),
                                                  do_group_parser()
                              ),
                              PARSER_CMB_SEQUENCE(7,
                                                  For_parser(),
                                                  name_parser(),
                                                  linebreak_parser(),
                                                  in_parser(),
                                                  wordlist_parser(),
                                                  sequential_sep_parser(),
                                                  do_group_parser()
                              )
            ),
            FOR_CLAUSE_PARSER);
}

/*
name             : NAME                     // Apply rule 5
                  ;
*/
static Combinator name_parser()
{
    return typed_cmb(
            NAME_parser(),
            NAME_PARSER);
}

/*
in               : In                       // Apply rule 6
                  ;
*/
static Combinator in_parser()
{
    return typed_cmb(
            In_parser(),
            IN_PARSER);
}

/*
wordlist         : wordlist WORD
                 |          WORD
                  ;
*/
static Combinator wordlist_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(2,
                              PARSER_CMB_SEQUENCE(2,
                                      PARSER_CMB_FORWARD_REF(wordlist_parser),
                                      WORD_parser()
                              ),
                              WORD_parser()
            ),
            WORDLIST_PARSER);
}

/*
case_clause      : Case WORD linebreak in linebreak case_list    Esac
                 | Case WORD linebreak in linebreak case_list_ns Esac
                 | Case WORD linebreak in linebreak              Esac
                  ;
*/
static Combinator case_clause_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(3,
                              PARSER_CMB_SEQUENCE(7,
                                                  Case_parser(),
                                                  WORD_parser(),
                                                  linebreak_parser(),
                                                  in_parser(),
                                                  linebreak_parser(),
                                                  case_list_parser(),
                                                  Esac_parser()
                              ),
                              PARSER_CMB_SEQUENCE(7,
                                                  Case_parser(),
                                                  WORD_parser(),
                                                  linebreak_parser(),
                                                  in_parser(),
                                                  linebreak_parser(),
                                                  case_list_ns_parser(),
                                                  Esac_parser()
                              ),
                              PARSER_CMB_SEQUENCE(6,
                                                  Case_parser(),
                                                  WORD_parser(),
                                                  linebreak_parser(),
                                                  in_parser(),
                                                  linebreak_parser(),
                                                  Esac_parser()
                              )
            ),
            CASE_CLAUSE_PARSER);
}

/*
case_list_ns     : case_list case_item_ns
                 |           case_item_ns
                 ;
*/
static Combinator case_list_ns_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(2,
                        PARSER_CMB_SEQUENCE(2,
                                case_list_parser(),
                                case_item_ns_parser()
                        ),
                        case_item_ns_parser()
            ),
            CASE_LIST_NS_PARSER);
}

/*
case_list        : case_list case_item
                 |           case_item
                 ;
*/
static Combinator case_list_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(2,
                      PARSER_CMB_SEQUENCE(2,
                                          PARSER_CMB_FORWARD_REF(case_list_parser),
                                          case_item_parser()
                      ),
                      case_item_parser()
            ),
            CASE_LIST_PARSER);
}

/*
case_item_ns     :     pattern ')' linebreak
                 |     pattern ')' compound_list
                 | '(' pattern ')' linebreak
                 | '(' pattern ')' compound_list
                 ;
*/
static Combinator case_item_ns_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(4,
                              PARSER_CMB_SEQUENCE(3,
                                      pattern_parser(),
                                      sub_Closing_Parenthesis_parser(),
                                      linebreak_parser()
                              ),
                                      PARSER_CMB_SEQUENCE(3,
                                      pattern_parser(),
                                      sub_Closing_Parenthesis_parser(),
                                      compound_list_parser()
                              ),
                                      PARSER_CMB_SEQUENCE(4,
                                      sub_Opening_Parenthesis_parser(),
                                      pattern_parser(),
                                      sub_Closing_Parenthesis_parser(),
                                      linebreak_parser()
                              ),
                                      PARSER_CMB_SEQUENCE(4,
                                      sub_Opening_Parenthesis_parser(),
                                      pattern_parser(),
                                      sub_Closing_Parenthesis_parser(),
                                      compound_list_parser()
                              )
            ),
            CASE_ITEM_NS_PARSER);
}

/*
case_item        :     pattern ')' linebreak     DSEMI linebreak
                 |     pattern ')' compound_list DSEMI linebreak
                 | '(' pattern ')' linebreak     DSEMI linebreak
                 | '(' pattern ')' compound_list DSEMI linebreak
                 ;
*/
static Combinator case_item_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(4,
                          PARSER_CMB_SEQUENCE(5,
                                              pattern_parser(),
                                              sub_Closing_Parenthesis_parser(),
                                  linebreak_parser(),
                                  DSEMI_parser(),
                                  linebreak_parser()
                          ),
                          PARSER_CMB_SEQUENCE(5,
                                  pattern_parser(),
                                  sub_Closing_Parenthesis_parser(),
                                  compound_list_parser(),
                                  DSEMI_parser(),
                                  linebreak_parser()
                          ),
                          PARSER_CMB_SEQUENCE(6,
                                  sub_Opening_Parenthesis_parser(),
                                  pattern_parser(),
                                  sub_Closing_Parenthesis_parser(),
                                  linebreak_parser(),
                                  DSEMI_parser(),
                                  linebreak_parser()
                          ),
                          PARSER_CMB_SEQUENCE(6,
                                  sub_Opening_Parenthesis_parser(),
                                  pattern_parser(),
                                  sub_Closing_Parenthesis_parser(),
                                  compound_list_parser(),
                                  DSEMI_parser(),
                                  linebreak_parser()
                          )
            ),
            CASE_ITEM_PARSER);
}

/*
pattern          :             WORD         // Apply rule 4
                 | pattern '|' WORD         // Do not apply rule 4
                 ;
*/
static Combinator pattern_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(2,
                      WORD_parser(),
                      PARSER_CMB_SEQUENCE(3,
                                          PARSER_CMB_FORWARD_REF(pattern_parser),
                                          sub_Pipe_parser(),
                              WORD_parser()
                      )
            ),
            PATTERN_PARSER);
}

/*
if_clause        : If compound_list Then compound_list else_part Fi
                 | If compound_list Then compound_list           Fi
                 ;
*/
static Combinator if_clause_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(2,
                              PARSER_CMB_SEQUENCE(6,
                                      If_parser(),
                                      compound_list_parser(),
                                      Then_parser(),
                                      compound_list_parser(),
                                      else_part_parser(),
                                      Fi_parser()
                              ),
                              PARSER_CMB_SEQUENCE(5,
                                      If_parser(),
                                      compound_list_parser(),
                                      Then_parser(),
                                      compound_list_parser(),
                                      Fi_parser()
                              )
            ),
            IF_CLAUSE_PARSER);
}

/*
else_part        : Elif compound_list Then compound_list
                 | Elif compound_list Then compound_list else_part
                 | Else compound_list
                 ;
*/
static Combinator else_part_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(3,
                              PARSER_CMB_SEQUENCE(4,
                                                  Elif_parser(),
                                                  compound_list_parser(),
                                                  Then_parser(),
                                                  compound_list_parser()
                              ),
                              PARSER_CMB_SEQUENCE(5,
                                                  Elif_parser(),
                                                  compound_list_parser(),
                                                  Then_parser(),
                                                  compound_list_parser(),
                                                  PARSER_CMB_FORWARD_REF(else_part_parser)
                              ),
                              PARSER_CMB_SEQUENCE(2,
                                                  Else_parser(),
                                                  compound_list_parser()
                              )
            ),
            ELSE_PART_PARSER);
}

/*

while_clause     : While compound_list do_group
                 ;
*/
static Combinator while_clause_parser()
{
    return typed_cmb(
            PARSER_CMB_SEQUENCE(3,
                                While_parser(),
                                compound_list_parser(),
                                do_group_parser()
            ),
            WHILE_CLAUSE_PARSER);
}

/*

until_clause     : Until compound_list do_group
                 ;
*/
static Combinator until_clause_parser()
{
    return typed_cmb(
            PARSER_CMB_SEQUENCE(3,
                                Until_parser(),
                                compound_list_parser(),
                                do_group_parser()
            ),
            UNTIL_CLAUSE_PARSER);
}

/*

function_definition : fname '(' ')' linebreak function_body
                 ;
*/
static Combinator function_definition_parser()
{
    return typed_cmb(
            PARSER_CMB_SEQUENCE(5,
                        fname_parser(),
                        sub_Opening_Parenthesis_parser(),
                        sub_Closing_Parenthesis_parser(),
                        linebreak_parser(),
                        function_body_parser()
            ),
            FUNCTION_DEFINITION_PARSER);
}

/*
function_body    : compound_command                // Apply rule 9
                 | compound_command redirect_list  // Apply rule 9
                 ;
*/
static Combinator function_body_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(2,
                        compound_command_parser(),
                        PARSER_CMB_SEQUENCE(2,
                              compound_command_parser(),
                              redirect_list_parser()
                        )
            ),
            FUNCTION_BODY_PARSER);
}

/*
fname            : NAME                            // Apply rule 8
                 ;
*/
static Combinator fname_parser()
{
    return typed_cmb(
            NAME_parser(),
            FNAME_PARSER);
}

/*
brace_group      : Lbrace compound_list Rbrace
                 ;
*/
static Combinator brace_group_parser()
{
    return typed_cmb(
            PARSER_CMB_SEQUENCE(3,
                        Lbrace_parser(),
                        compound_list_parser(),
                        Rbrace_parser()
            ),
            BRACE_GROUP_PARSER);
}

/*
do_group         : Do compound_list Done           // Apply rule 6
                 ;
*/
static Combinator do_group_parser()
{
    return typed_cmb(
            PARSER_CMB_SEQUENCE(3,
                                Do_parser(),
                                compound_list_parser(),
                                Done_parser()
            ),
            DO_GROUP_PARSER);
}

/*
simple_command   : cmd_prefix cmd_word cmd_suffix
                 | cmd_prefix cmd_word
                 | cmd_prefix
                 | cmd_name cmd_suffix
                 | cmd_name
                 ;
*/
static Combinator simple_command_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(5,
                        PARSER_CMB_SEQUENCE(3,
                              cmd_prefix_parser(),
                              cmd_word_parser(),
                              cmd_suffix_parser()
                        ),
                        PARSER_CMB_SEQUENCE(2,
                              cmd_prefix_parser(),
                              cmd_word_parser()
                        ),
                        cmd_prefix_parser(),
                        PARSER_CMB_SEQUENCE(2,
                              cmd_name_parser(),
                              cmd_suffix_parser()
                        ),
                        cmd_name_parser()
            ),
            SIMPLE_COMMAND_PARSER);
}

/*
cmd_name         : WORD                   // Apply rule 7a
                 ;
*/
static Combinator cmd_name_parser()
{
    return typed_cmb(
            WORD_parser(),
            CMD_NAME_PARSER);
}

/*
cmd_word         : WORD                   // Apply rule 7b
                 ;
*/
static Combinator cmd_word_parser()
{
    return typed_cmb(
            WORD_parser(),
            CMD_WORD_PARSER);
}

/*
cmd_prefix       :            io_redirect
                 | cmd_prefix io_redirect
                 |            ASSIGNMENT_WORD
                 | cmd_prefix ASSIGNMENT_WORD
                 ;
*/
static Combinator cmd_prefix_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(4,
                        io_redirect_parser(),
                        PARSER_CMB_SEQUENCE(2,
                              PARSER_CMB_FORWARD_REF(cmd_prefix_parser),
                              io_redirect_parser()
                        ),
                        ASSIGNMENT_WORD_parser(),
                        PARSER_CMB_SEQUENCE(2,
                              PARSER_CMB_FORWARD_REF(cmd_prefix_parser),
                              ASSIGNMENT_WORD_parser()
                        )
            ),
            CMD_PREFIX_PARSER);
}

/*
cmd_suffix       :            io_redirect
                 | cmd_suffix io_redirect
                 |            WORD
                 | cmd_suffix WORD
                 ;
*/
static Combinator cmd_suffix_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(4,
                        io_redirect_parser(),
                        PARSER_CMB_SEQUENCE(2,
                                PARSER_CMB_FORWARD_REF(cmd_suffix_parser),
                                io_redirect_parser()
                        ),
                        WORD_parser(),
                        PARSER_CMB_SEQUENCE(2,
                                PARSER_CMB_FORWARD_REF(cmd_suffix_parser),
                                WORD_parser()
                        )
            ),
            CMD_SUFFIX_PARSER);
}

/*
redirect_list    :               io_redirect
                 | redirect_list io_redirect
                 ;
*/
static Combinator redirect_list_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(2,
                        io_redirect_parser(),
                        PARSER_CMB_SEQUENCE(2,
                                PARSER_CMB_FORWARD_REF(redirect_list_parser),
                                io_redirect_parser()
                        )
            ),
            REDIRECT_LIST_PARSER);
}

/*
io_redirect      :           io_file
                 | IO_NUMBER io_file
                 |           io_here
                 | IO_NUMBER io_here
                 ;
*/
static Combinator io_redirect_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(4,
                      io_file_parser(),
                      PARSER_CMB_SEQUENCE(2,
                              IO_NUMBER_parser(),
                              io_file_parser()
                      ),
                      io_here_parser(),
                      PARSER_CMB_SEQUENCE(2,
                              IO_NUMBER_parser(),
                              io_here_parser()
                      )
            ),
            IO_REDIRECT_PARSER);
}

/*
io_file          : '<'       filename
                 | LESSAND   filename
                 | '>'       filename
                 | GREATAND  filename
                 | DGREAT    filename
                 | LESSGREAT filename
                 | CLOBBER   filename
                 ;
*/
static Combinator io_file_parser()
{
    return typed_cmb(
    PARSER_CMB_CHOICE(7,
                      PARSER_CMB_SEQUENCE(2,
                              sub_Less_parser(),
                              filename_parser()
                      ),
                      PARSER_CMB_SEQUENCE(2,
                              LESSAND_parser(),
                              filename_parser()
                      ),
                      PARSER_CMB_SEQUENCE(2,
                              sub_Greater_parser(),
                              filename_parser()
                      ),
                      PARSER_CMB_SEQUENCE(2,
                              GREATAND_parser(),
                              filename_parser()
                      ),
                      PARSER_CMB_SEQUENCE(2,
                              DGREAT_parser(),
                              filename_parser()
                      ),
                      PARSER_CMB_SEQUENCE(2,
                              LESSGREAT_parser(),
                              filename_parser()
                      ),
                      PARSER_CMB_SEQUENCE(2,
                              CLOBBER_parser(),
                              filename_parser()
                      )
    ),
    IO_FILE_PARSER);
}

/*
filename         : WORD                      // Apply rule 2
                 ;
*/
static Combinator filename_parser()
{
    return typed_cmb(
            WORD_parser(),
            FILENAME_PARSER);
}

/*

io_here          : DLESS     here_end
                 | DLESSDASH here_end
                 ;
*/
static Combinator io_here_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(2,
                        PARSER_CMB_SEQUENCE(2,
                              DLESS_parser(),
                              here_end_parser()
                        ),
                        PARSER_CMB_SEQUENCE(2,
                              DLESSDASH_parser(),
                              here_end_parser()
                        )
            ),
            IO_HERE_PARSER);
}

/*
here_end         : WORD                      // Apply rule 3
                 ;
*/
static Combinator here_end_parser()
{
    return typed_cmb(
            WORD_parser(),
            HERE_END_PARSER);
}

/*
newline_list     :              NEWLINE
                 | newline_list NEWLINE
                 ;
*/
static Combinator newline_list_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(2,
                        NEWLINE_parser(),
                        PARSER_CMB_SEQUENCE(2,
                                PARSER_CMB_FORWARD_REF(newline_list_parser),
                                NEWLINE_parser()
                        )
            ),
            NEWLINE_LIST_PARSER);
}

/*
linebreak        : newline_list
                 | // empty
                 ;
*/
static Combinator linebreak_parser()
{
    return typed_cmb(
            cmb_optional(
                    newline_list_parser()
            ),
            LINEBREAK_PARSER);
}

/*
separator_op     : '&'
                 | ';'
                 ;
*/
static Combinator separator_op_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(2,
                        sub_Ampersand_parser(),
                        sub_Semicolon_parser()
            ),
            SEPARATOR_OP_PARSER);
}

/*
separator        : separator_op linebreak
                 | newline_list
                 ;
*/
static Combinator separator_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(2,
                        PARSER_CMB_SEQUENCE(2,
                                          separator_op_parser(),
                                          linebreak_parser()
                        ),
                        newline_list_parser()
            ),
            SEPARATOR_PARSER);
}

/*
sequential_sep   : ';' linebreak
                 | newline_list
                 ;
*/
static Combinator sequential_sep_parser()
{
    return typed_cmb(
            PARSER_CMB_CHOICE(2,
                        PARSER_CMB_SEQUENCE(2,
                                sub_Semicolon_parser(),
                                linebreak_parser()
                        ),
                        newline_list_parser()
            ),
            SEQUENTIAL_SEP_PARSER);
}

/* Token Parsers */
static Combinator WORD_parser(void)
{
    Combinator cmb = {};
    return typed_cmb(
            cmb,
            TK_WORD_PARSER);
}

static Combinator ASSIGNMENT_WORD_parser(void)
{
    Combinator cmb = {};
    return typed_cmb(
            cmb,
            TK_ASSIGNMENT_WORD_PARSE);
}

static Combinator NAME_parser(void)
{
    Combinator cmb = {};
    return typed_cmb(
            cmb,
            TK_NAME_PARSER);
}

static Combinator NEWLINE_parser(void)
{
    Combinator cmb = {};
    return typed_cmb(
            cmb,
            TK_NEWLINE_PARSER);
}

static Combinator IO_NUMBER_parser(void)
{
    Combinator cmb = {};
    return typed_cmb(
            cmb,
            TK_IO_NUMBER);
}
