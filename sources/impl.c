//
// Created on 17/05/2022.
//

#include <string.h>
#include "impl.h"
#include "parser/parser.h"

static Combinator command_parser();
static Combinator command_unit_parser();

static Combinator command_prefix_parser();
static Combinator scope_command_parser();
static Combinator classic_command_parser();

static Combinator name_parser();
static Combinator names_parser();

static Combinator redirect_parser();
static Combinator redirect_in_parser();
static Combinator redirect_out_parser();

static Combinator cmd_sep_parser();
static Combinator pipe_parser();
static Combinator amp_parser();
static Combinator new_cmd_parser();

static Combinator literal_parser();
static Combinator doubleq_parser();
static Combinator singleq_parser();
static Combinator reserved_keyword_parser();

static Combinator if_statement_parser();
static Combinator for_loop_parser();
static Combinator while_loop_parser();
static Combinator until_loop_parser();
static Combinator case_statement_parser();


// Utils
#define STRING_PARSER(name, match) \
static bool _gen_string_parser_##name##_parse_f(void * void_ctx, Combinator * p) \
{ \
    (void)p; \
    ParseContext * ctx = void_ctx; \
    Token token = ctx->tokens[ctx->pos++]; \
    return strcmp(token.str, match) == 0; \
} \
static Combinator gen_string_parser_##name() \
{ \
    return typed_cmb(cmb_create(parser_parse, _gen_string_parser_##name##_parse_f, parser_commit_single_token), CST_STRING_PARSER); \
} \

#define STRING_AND_STATE_PARSER(name, match_str, match_state) \
static bool _gen_string_and_state_parser_##name##_parse_f(void * void_ctx, Combinator * p) \
{ \
    (void)p; \
    ParseContext * ctx = void_ctx; \
    Token token = ctx->tokens[ctx->pos++]; \
    return token.state == (match_state) && strcmp(token.str, match_str) == 0; \
} \
static Combinator gen_string_and_state_parser_##name() \
{ \
    return typed_cmb(cmb_create(parser_parse, _gen_string_and_state_parser_##name##_parse_f, parser_commit_single_token), CST_STRING_STATE_PARSER); \
} \

#define STATE_PARSER(name, match_state) \
static bool _gen_state_parser_##name##_parse_f(void * void_ctx, Combinator * p) \
{ \
    (void)p; \
    ParseContext * ctx = void_ctx; \
    Token token = ctx->tokens[ctx->pos++]; \
    return token.state == (match_state); \
} \
static Combinator gen_state_parser_##name() \
{ \
    return typed_cmb(cmb_create(parser_parse, _gen_state_parser_##name##_parse_f, parser_commit_single_token), CST_STATE_PARSER); \
} \

#define STRING_PARSER_SELF(x) STRING_PARSER(x, #x)

STRING_PARSER(equal_sign, "=")
STRING_PARSER(opening_parenthesis, "(")
STRING_PARSER(closing_parenthesis, ")")
STRING_PARSER(single_pipe, "|")

STRING_AND_STATE_PARSER(new_line, "\n", STATE_SINGLE_CHAR)
STRING_AND_STATE_PARSER(semicolon, ";", STATE_SEMICOLON)
STRING_AND_STATE_PARSER(double_semicolon, ";;", STATE_SEMICOLON)

STATE_PARSER(right_angle_bracket, STATE_RIGHT_ANGLE_BRACKET)
STATE_PARSER(left_angle_bracket, STATE_LEFT_ANGLE_BRACKET)
STATE_PARSER(doubleq, STATE_DOUBLE_QUOTES)
STATE_PARSER(singleq, STATE_SINGLE_QUOTES)
STATE_PARSER(pipe, STATE_PIPE)
STATE_PARSER(amp, STATE_AMPERSAND)

STRING_PARSER_SELF(if)
STRING_PARSER_SELF(then)
STRING_PARSER_SELF(else)
STRING_PARSER_SELF(elif)
STRING_PARSER_SELF(fi)
STRING_PARSER_SELF(while)
STRING_PARSER_SELF(for)
STRING_PARSER_SELF(in)
STRING_PARSER_SELF(until)
STRING_PARSER_SELF(do)
STRING_PARSER_SELF(done)
STRING_PARSER_SELF(case)
STRING_PARSER_SELF(esac)

// Oten-used micro-parsers
static Combinator lookahead_cmd_sep()
{
    return cmb_lookahead(parser_parse, cmd_sep_parser());
}

static Combinator optional_new_line_parser()
{
    return cmb_optional(gen_string_and_state_parser_new_line());
}

// Main
Combinator shell_instruction_parser()
{
    return typed_cmb(
            cmb_separated(
                        parser_parse,
                        command_parser(),
                        cmd_sep_parser()
            ),
            CST_SHELL_INSTRUCTION);
}

static Combinator command_parser()
{
    return typed_cmb(
            cmb_sequence(parser_parse, 2,
                         command_unit_parser(),
                         redirect_parser()
            ),
            CST_COMMAND);
}

static Combinator command_unit_parser()
{
    return typed_cmb(
            cmb_choice(parser_parse, 2,
                       scope_command_parser(),
                       classic_command_parser()
            ),
            CST_COMMAND_UNIT);
}

static Combinator command_prefix_parser()
{
    return typed_cmb(
            cmb_sequence(parser_parse, 4,
                         literal_parser(),
                         gen_string_parser_equal_sign(),
                         name_parser(),
                         cmb_inverted(
                                 lookahead_cmd_sep()
                         )
            ),
            CST_COMMAND_PREFIX);
}

static Combinator scope_command_parser()
{
    return typed_cmb(
            cmb_choice(parser_parse, 5,
                       if_statement_parser(),
                       for_loop_parser(),
                       while_loop_parser(),
                       until_loop_parser(),
                       case_statement_parser()
            ),
            CST_SCOPE_COMMAND);
}

static Combinator classic_command_parser()
{
    return typed_cmb(
            cmb_sequence(parser_parse, 3,
                         cmb_repetition(
                                 command_prefix_parser()
                         ),
                         cmb_lookahead(parser_parse,
                                       cmb_inverted(
                                         cmb_sequence(parser_parse, 2,
                                                      reserved_keyword_parser(),
                                                      cmd_sep_parser()
                                         )
                                 )
                         ),
                         names_parser()
            ),
            CST_CLASSIC_COMMAND);
}

static Combinator name_parser()
{
    return typed_cmb(
            cmb_choice(parser_parse, 4,
                       literal_parser(),
                       doubleq_parser(),
                       singleq_parser(),
                       gen_string_parser_equal_sign()
            ),
            CST_NAME);
}

static Combinator names_parser()
{
    return typed_cmb(
            cmb_sequence(parser_parse, 2,
                         name_parser(),
                         cmb_repetition(
                                 name_parser()
                         )
            ),
            CST_NAMES);
}

static Combinator redirect_parser()
{
    return typed_cmb(
            cmb_repetition(
                    cmb_choice(parser_parse, 2,
                               redirect_in_parser(),
                               redirect_out_parser()
                    )
            ),
            CST_REDIRECT);
}

static Combinator redirect_in_parser()
{
    return typed_cmb(
            cmb_sequence(parser_parse, 2,
                         gen_state_parser_left_angle_bracket(),
                         name_parser()
            ),
            CST_REDIRECT_IN);
}

static Combinator redirect_out_parser()
{
    return typed_cmb(
            cmb_sequence(parser_parse, 2,
                         gen_state_parser_right_angle_bracket(),
                         name_parser()
            ),
            CST_REDIRECT_OUT);
}

static Combinator cmd_sep_parser()
{
    return typed_cmb(
            cmb_choice(parser_parse, 3,
                       pipe_parser(),
                       amp_parser(),
                       new_cmd_parser()
            ),
            CST_CMD_SEP);
}

static Combinator pipe_parser()
{
    return typed_cmb(
            gen_state_parser_pipe(),
            CST_PIPE);
}

static Combinator amp_parser()
{
    return typed_cmb(
            gen_state_parser_amp(),
            CST_AMP);
}

static Combinator new_cmd_parser()
{
    return typed_cmb(
            cmb_choice(parser_parse, 2,
                       gen_string_and_state_parser_semicolon(),
                       gen_string_and_state_parser_new_line()
            ),
            CST_NEW_CMD);
}

static bool literal_is_not_reserved(char * literal)
{
    return strcmp (literal, "!") != 0 && \
            strcmp(literal, "{") != 0 && \
            strcmp(literal, "}") != 0/* && \
            strcmp(literal, "case") != 0 && \
            strcmp(literal, "do") != 0 && \
            strcmp(literal, "done") != 0 && \
            strcmp(literal, "elif") != 0 && \
            strcmp(literal, "else") != 0 && \
            strcmp(literal, "esac") != 0 && \
            strcmp(literal, "fi") != 0 && \
            strcmp(literal, "for") != 0 && \
            strcmp(literal, "if") != 0 && \
            strcmp(literal, "in") != 0 && \
            strcmp(literal, "then") != 0 && \
            strcmp(literal, "until") != 0 && \
            strcmp(literal, "while") != 0*/;
}

static bool literal_parser_parse_f(void * void_ctx, Combinator * p)
{
    (void)p;
    ParseContext * ctx = void_ctx;
    Token token = ctx->tokens[ctx->pos++];
    return token.state == STATE_LITERAL && literal_is_not_reserved(token.str);
}

static Combinator literal_parser()
{
    return typed_cmb(
            cmb_create(parser_parse, literal_parser_parse_f, parser_commit_single_token),
            CST_LITERAL);
}

static Combinator doubleq_parser()
{
    return typed_cmb(
            gen_state_parser_doubleq(),
            CST_DOUBLEQ);
}

static Combinator singleq_parser()
{
    return typed_cmb(
            gen_state_parser_singleq(),
            CST_SINGLEQ);
}

static bool literal_is_reserved_full(char * literal)
{
    return strcmp (literal, "!") == 0 || \
            strcmp(literal, "{") == 0 || \
            strcmp(literal, "}") == 0 || \
            strcmp(literal, "case") == 0 || \
            strcmp(literal, "do") == 0 || \
            strcmp(literal, "done") == 0 || \
            strcmp(literal, "elif") == 0 || \
            strcmp(literal, "else") == 0 || \
            strcmp(literal, "esac") == 0 || \
            strcmp(literal, "fi") == 0 || \
            strcmp(literal, "for") == 0 || \
            strcmp(literal, "if") == 0 || \
            strcmp(literal, "in") == 0 || \
            strcmp(literal, "then") == 0 || \
            strcmp(literal, "until") == 0 || \
            strcmp(literal, "while") == 0;
}

static bool reserved_keyword_parser_parse_f(void * void_ctx, Combinator * p)
{
    (void)p;
    ParseContext * ctx = void_ctx;
    Token token = ctx->tokens[ctx->pos++];
    return token.state == STATE_LITERAL && literal_is_reserved_full(token.str);
}

static Combinator reserved_keyword_parser()
{
    return cmb_create(parser_parse, reserved_keyword_parser_parse_f, parser_commit_single_token);
}

static Combinator if_condition_action_parser()
{
    return typed_cmb(
            cmb_sequence(parser_parse, 6,
                         names_parser(),
                         new_cmd_parser(),
                         gen_string_parser_then(),
                         optional_new_line_parser(),
                         cmb_forward_ref(parser_parse, shell_instruction_parser),
                         new_cmd_parser()
            ),
            CST_IF_CONDITION_ACTION);
}

static Combinator if_alternative_parser()
{
    return typed_cmb(
            cmb_choice(parser_parse, 2,
                       cmb_sequence(parser_parse, 4,
                                    gen_string_parser_else(),
                                    gen_string_and_state_parser_new_line(),
                                    cmb_forward_ref(parser_parse, shell_instruction_parser),
                                    new_cmd_parser()
                       ),
                       cmb_sequence(parser_parse, 2,
                                    gen_string_parser_elif(),
                                    if_condition_action_parser()
                       )
            ),
            CST_IF_ALTERNATIVE);
}

static Combinator if_statement_parser()
{
    return typed_cmb(
            cmb_sequence(parser_parse, 5,
                         gen_string_parser_if(),
                         if_condition_action_parser(),
                         cmb_repetition(
                                 if_alternative_parser()
                         ),
                         gen_string_parser_fi(),
                         lookahead_cmd_sep()
            ),
            CST_IF_STATEMENT);
}

static Combinator loop_body_parser()
{
    return typed_cmb(
            cmb_sequence(parser_parse, 7,
                         new_cmd_parser(),
                         gen_string_parser_do(),
                         optional_new_line_parser(),
                         cmb_forward_ref(parser_parse, shell_instruction_parser),
                         new_cmd_parser(),
                         gen_string_parser_done(),
                         lookahead_cmd_sep()
            ),
            CST_LOOP_BODY);
}

static Combinator for_loop_parser()
{
    return typed_cmb(
            cmb_sequence(parser_parse, 4,
                         gen_string_parser_for(),
                         literal_parser(),
                         cmb_optional(
                                 cmb_sequence(parser_parse, 2,
                                              gen_string_parser_in(),
                                              names_parser()
                                 )
                         ),
                         loop_body_parser()
            ),
            CST_FOR_LOOP);
}

static Combinator while_loop_parser()
{
    return typed_cmb(
            cmb_sequence(parser_parse, 3,
                         gen_string_parser_while(),
                         names_parser(),
                         loop_body_parser()
            ),
            CST_WHILE_LOOP);
}

static Combinator until_loop_parser()
{
    return typed_cmb(
            cmb_sequence(parser_parse, 3,
                         gen_string_parser_until(),
                         names_parser(),
                         loop_body_parser()
            ),
            CST_UNTIL_LOOP);
}

static Combinator case_expr_parser()
{
    return typed_cmb(
            cmb_sequence(parser_parse, 6,
                         optional_new_line_parser(),
                         cmb_optional(gen_string_parser_opening_parenthesis()),
                         cmb_separated(
                                 parser_parse,
                                 name_parser(),
                                 gen_string_parser_single_pipe()
                         ),
                         gen_string_parser_closing_parenthesis(),
                         optional_new_line_parser(),
                         cmb_forward_ref(parser_parse, shell_instruction_parser)
            ),
            CST_CASE_EXPR);
}

static Combinator case_statement_parser()
{
    return typed_cmb(
            cmb_sequence(parser_parse, 6,
                         gen_string_parser_case(),
                         literal_parser(),
                         gen_string_parser_in(),
                         cmb_optional(
                                 cmb_choice(parser_parse, 2,
                                            cmb_sequence(parser_parse, 2,
                                                         cmb_separated(
                                                                 parser_parse,
                                                                 case_expr_parser(),
                                                                 gen_string_and_state_parser_double_semicolon()
                                                         ),
                                                         cmb_choice(parser_parse, 2,
                                                                    cmb_sequence(parser_parse, 3,
                                                                                 optional_new_line_parser(),
                                                                                 gen_string_and_state_parser_double_semicolon(),
                                                                                 optional_new_line_parser()
                                                                    ),
                                                                    gen_string_and_state_parser_new_line()
                                                         )
                                            ),
                                            gen_string_and_state_parser_new_line()
                                 )
                         ),
                         gen_string_parser_esac(),
                         lookahead_cmd_sep()
            ),
            CST_CASE_STATEMENT);
}
