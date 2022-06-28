//
// Created on 17/05/2022.
//

// Using https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "impl.h"
#include "parser/parser.h"
#include "lexer/shell_grammar/rules.h"

// Grammar for reserved words
const char * const GRAMMAR_RESERVED_WORDS[] = {
        "if", "then", "else", "elif",
        "fi", "do", "done", "case",
        "esac", "while", "until", "for",
        "{", "}", "!", "in",
};

const size_t NUM_GRAMMAR_RESERVED_WORDS = sizeof(GRAMMAR_RESERVED_WORDS) / sizeof(char *);

const TokenType GRAMMAR_RESERVED_WORDS_TYPES[] = {
        IF_TOKEN, THEN_TOKEN, ELSE_TOKEN, ELIF_TOKEN,
        FI_TOKEN, DO_TOKEN, DONE_TOKEN, CASE_TOKEN,
        ESAC_TOKEN, WHILE_TOKEN, UNTIL_TOKEN, FOR_TOKEN,
        OPENING_BRACKET_TOKEN, CLOSING_BRACKET_TOKEN, BANG_TOKEN, IN_TOKEN,
};

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
    return typed_cmb(PARSER_CREATE(_gen_string_parser_##name##_parse_f, parser_commit_single_token), GEN_STRING_PARSER); \
} \


#define STRING_PARSER_RULE_1(name, match, token_type) \
static bool _gen_string_parser_##name##_parse_f(void * void_ctx, Combinator * p) \
{ \
    (void)p; \
    ParseContext * ctx = void_ctx; \
    Token token = ctx->tokens[ctx->pos++]; \
    return strcmp(token.str, match) == 0; \
} \
static bool name##_GRAMMAR_RULE_1_decorator(void * void_ctx, Combinator * cmb) { \
    bool success = cmb->exec_f(void_ctx, cmb);        \
    if (success) {                                    \
        ParseContext * ctx = void_ctx;                  \
        ctx->tokens[ctx->pos].type = token_type; \
    }                                                 \
    return success; \
} \
static Combinator name##_parser() \
{ \
    Combinator cmb = typed_cmb(PARSER_CREATE(_gen_string_parser_##name##_parse_f, parser_commit_single_token), GEN_STRING_R1_PARSER); \
    cmb.decorator = name##_GRAMMAR_RULE_1_decorator;  \
    return cmb; \
} \


#define TOKEN_TYPE_PARSER(name) \
static bool _gen_token_parser_##name##_parse_f(void * void_ctx, Combinator * p) \
{ \
    (void)p; \
    ParseContext * ctx = void_ctx; \
    Token token = ctx->tokens[ctx->pos++]; \
    return token.type == name##_TOKEN; \
} \
static Combinator name##_parser() \
{ \
    return typed_cmb(PARSER_CREATE(_gen_token_parser_##name##_parse_f, parser_commit_single_token), TK_##name##_PARSER); \
} \



/* OFFICIAL GRAMMAR IMPLEMENTATION */

/* -------------------------------------------------------
   The grammar symbols
   ------------------------------------------------------- */
// static Combinator WORD_parser(void);
// TOKEN_TYPE_PARSER(WORD)
static _Bool _gen_token_parser_WORD_parse_f(void * void_ctx, Combinator * p) {
    (void) p;
    ParseContext *ctx = void_ctx;
    Token token = ctx->tokens[ctx->pos++];
    // special case for TOKEN_TOKEN (or NONE_TOKEN)
    if (token.type == NONE_TOKEN) {
        fprintf(stderr, "None token cannot be parsed\n");
        exit(1);
    }
    if (token.type == TOKEN_TOKEN) {
        ctx->pos--;
        token.type = NONE_TOKEN;
        return GRAMMAR_RULE_DECORATOR[GRAMMAR_RULE_1](ctx, p);
    }
    return token.type == WORD_TOKEN;
}
static Combinator WORD_parser() {
    return typed_cmb((cmb_create(parser_parse, _gen_token_parser_WORD_parse_f, parser_commit_single_token)), TK_WORD_PARSER);
}

TOKEN_TYPE_PARSER(ASSIGNMENT_WORD)
TOKEN_TYPE_PARSER(NAME)
TOKEN_TYPE_PARSER(NEWLINE)
TOKEN_TYPE_PARSER(IO_NUMBER)

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
STRING_PARSER_RULE_1(If, "if", IF_TOKEN)
STRING_PARSER_RULE_1(Then, "then", THEN_TOKEN)
STRING_PARSER_RULE_1(Else, "else", ELSE_TOKEN)
STRING_PARSER_RULE_1(Elif, "elif", ELIF_TOKEN)
STRING_PARSER_RULE_1(Fi, "fi", FI_TOKEN)
STRING_PARSER_RULE_1(Do, "do", DO_TOKEN)
STRING_PARSER_RULE_1(Done, "done", DONE_TOKEN)

STRING_PARSER_RULE_1(Case, "case", CASE_TOKEN)
STRING_PARSER_RULE_1(Esac, "esac", ESAC_TOKEN)
STRING_PARSER_RULE_1(While, "while", WHILE_TOKEN)
STRING_PARSER_RULE_1(Until, "until", UNTIL_TOKEN)
STRING_PARSER_RULE_1(For, "for", FOR_TOKEN)

/* These are reserved words, not operator tokens, and are
    recognized when reserved words are recognized. */
STRING_PARSER(Lbrace, "{")
STRING_PARSER(Rbrace, "}")
STRING_PARSER(Bang, "!")

STRING_PARSER_RULE_1(In, "in", IN_TOKEN)

/* Some more parsers for simple string/char parsing */
STRING_PARSER(sub_Opening_Parenthesis, "(")
STRING_PARSER(sub_Pipe, "|")
STRING_PARSER(sub_Closing_Parenthesis, ")")
STRING_PARSER(sub_Less, "<")
STRING_PARSER(sub_Greater, ">")
STRING_PARSER(sub_Ampersand, "&")
STRING_PARSER(sub_Semicolon, ";")

/* Apply grammar rules to parser */
static Combinator apply_rule(GRAMMAR_RULE rule, Combinator cmb) {
    cmb.decorator = GRAMMAR_RULE_DECORATOR[rule];
    return cmb;
}

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
            PARSER_CMB_SEPARATED(
                    complete_command_parser(),
                    newline_list_parser()
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
                    PARSER_CMB_SEQUENCE(2,
                            list_parser(),
                            separator_op_parser()
                    ),
                    list_parser()
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
            PARSER_CMB_SEPARATED(
                      and_or_parser(),
                      separator_op_parser()
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
            PARSER_CMB_SEPARATED(
                    // giant recursion occurring if called directly
                    PARSER_CMB_FORWARD_REF(pipeline_parser),
                    PARSER_CMB_SEQUENCE(2,
                            PARSER_CMB_CHOICE(2,
                                    AND_IF_parser(),
                                    OR_IF_parser()
                            ),
                            linebreak_parser()
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
            PARSER_CMB_SEPARATED(
                    command_parser(),
                    PARSER_CMB_SEQUENCE(2,
                            sub_Pipe_parser(),
                            linebreak_parser()
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
            PARSER_CMB_SEPARATED(
                    separator_parser(),
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
            apply_rule(
                    GRAMMAR_RULE_5,
                    NAME_parser()
            ),
            NAME_PARSER);
}

/*
in               : In                       // Apply rule 6
                 ;
*/
static Combinator in_parser()
{
    return typed_cmb(
            apply_rule(
                    GRAMMAR_RULE_6,
                    In_parser()
            ),
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
            PARSER_CMB_ONE_OR_MORE(
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
            PARSER_CMB_ONE_OR_MORE(
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
                    PARSER_CMB_SEQUENCE(2,
                            WORD_parser(),
                            PARSER_CMB_INVERTED(
                                    PARSER_CMB_LOOKAHEAD(
                                            sub_Pipe_parser()
                                    )
                            )
                    ),
                    PARSER_CMB_SEPARATED(
                            WORD_parser(),
                            sub_Pipe_parser()
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
            apply_rule(
                    GRAMMAR_RULE_9,
                    PARSER_CMB_CHOICE(2,
                            compound_command_parser(),
                            PARSER_CMB_SEQUENCE(2,
                                  compound_command_parser(),
                                  redirect_list_parser()
                            )
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
            apply_rule(
                    GRAMMAR_RULE_8,
                    NAME_parser()
            ),
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
            apply_rule(
                    GRAMMAR_RULE_6,
                    PARSER_CMB_SEQUENCE(3,
                                Do_parser(),
                                compound_list_parser(),
                                Done_parser()
                    )
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
            apply_rule(
                    GRAMMAR_RULE_7a,
                    WORD_parser()
            ),
            CMD_NAME_PARSER);
}

/*
cmd_word         : WORD                   // Apply rule 7b
                 ;
*/
static Combinator cmd_word_parser()
{
    return typed_cmb(
            apply_rule(
                    GRAMMAR_RULE_7b,
                    WORD_parser()
            ),
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
            PARSER_CMB_ONE_OR_MORE(
                        PARSER_CMB_CHOICE(2,
                                io_redirect_parser(),
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
            PARSER_CMB_ONE_OR_MORE(
                    PARSER_CMB_CHOICE(2,
                                      io_redirect_parser(),
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
            PARSER_CMB_ONE_OR_MORE(
                    io_redirect_parser()
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
            apply_rule(
                    GRAMMAR_RULE_2,
                    WORD_parser()
            ),
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
            apply_rule(
                    GRAMMAR_RULE_3,
                    WORD_parser()
            ),
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
            PARSER_CMB_ONE_OR_MORE(
                    NEWLINE_parser()
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
            PARSER_CMB_OPTIONAL(
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
/*
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
            TK_ASSIGNMENT_WORD_PARSER);
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
            TK_IO_NUMBER_PARSER);
}
*/
