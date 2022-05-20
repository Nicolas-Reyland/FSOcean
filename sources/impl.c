//
// Created on 17/05/2022.
//

#include <string.h>
#include "impl.h"

static Parser command_parser();
static Parser command_unit_parser();

static Parser command_prefix_parser();
static Parser scope_command_parser();

static Parser name_parser();
static Parser names_parser();

static Parser redirect_parser();
static Parser redirect_in_parser();
static Parser redirect_out_parser();

static Parser cmd_sep_parser();
static Parser pipe_parser();
static Parser amp_parser();
static Parser new_cmd_parser();

static Parser literal_parser();
static Parser doubleq_parser();
static Parser singleq_parser();

static Parser if_statement_parser();
static Parser for_loop_parser();
static Parser while_loop_parser();
static Parser until_loop_parser();
static Parser case_statement_parser();


// Utils
#define STRING_PARSER(name, match) \
static bool _gen_string_parser_##name##_parser(ParseContext * ctx, Parser * p) \
{ \
    (void)p; \
    Token token = ctx->tokens[ctx->pos++]; \
    return strcmp(token.str, match) == 0; \
} \
static Parser gen_string_parser_##name() \
{ \
    return typed_parser(create_parser(_gen_string_parser_##name##_parser, parser_commit_single_token), CST_STRING_PARSER); \
} \

#define STRING_AND_STATE_PARSER(name, match_str, match_state) \
static bool _gen_string_and_state_parser_##name##_parser(ParseContext * ctx, Parser * p) \
{ \
    (void)p; \
    Token token = ctx->tokens[ctx->pos++]; \
    return token.state == (match_state) && strcmp(token.str, match_str) == 0; \
} \
static Parser gen_string_and_state_parser_##name() \
{ \
    return typed_parser(create_parser(_gen_string_and_state_parser_##name##_parser, parser_commit_single_token), CST_STRING_STATE_PARSER); \
} \

#define STATE_PARSER(name, match_state) \
static bool _gen_state_parser_##name##_parser(ParseContext * ctx, Parser * p) \
{ \
    (void)p; \
    Token token = ctx->tokens[ctx->pos++]; \
    return token.state == (match_state); \
} \
static Parser gen_state_parser_##name() \
{ \
    return typed_parser(create_parser(_gen_state_parser_##name##_parser, parser_commit_single_token), CST_STATE_PARSER); \
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

// Main
Parser shell_instruction_parser()
{
    return typed_parser(
            separated_parser(
                        command_parser(),
                        cmd_sep_parser()
            ),
            CST_SHELL_INSTRUCTION);
}

static Parser command_parser()
{
    return typed_parser(
            parser_sequence(2,
                        command_unit_parser(),
                        redirect_parser()
            ),
            CST_COMMAND);
}

static Parser command_unit_parser()
{
    return typed_parser(
            parser_choice(2,
                        scope_command_parser(),
                        parser_sequence(2,
                                command_prefix_parser(),
                                names_parser()
                        )
            ),
            CST_COMMAND_UNIT);
}

static Parser command_prefix_parser()
{
    return typed_parser(
            parser_repetition(
                        parser_sequence(3,
                                literal_parser(),
                                gen_string_parser_equal_sign(),
                                name_parser()
                        )
            ),
            CST_COMMAND_PREFIX);
}

static Parser scope_command_parser()
{
    return typed_parser(
            parser_choice(5,
                        if_statement_parser(),
                        for_loop_parser(),
                        while_loop_parser(),
                        until_loop_parser(),
                        case_statement_parser()
            ),
            CST_SCOPE_COMMAND);
}

static Parser name_parser()
{
    return typed_parser(
            parser_choice(3,
                          literal_parser(),
                          doubleq_parser(),
                          singleq_parser()),
            CST_NAME);
}

static Parser names_parser()
{
    return typed_parser(
            parser_sequence(2,
                        name_parser(),
                        parser_repetition(
                                name_parser()
                        )
            ),
            CST_NAMES);
}

static Parser redirect_parser()
{
    return typed_parser(
            parser_repetition(
                        parser_choice(2,
                                redirect_in_parser(),
                                redirect_out_parser()
                        )
            ),
            CST_REDIRECT);
}

static Parser redirect_in_parser()
{
    return typed_parser(
            parser_sequence(2,
                        gen_state_parser_left_angle_bracket(),
                        name_parser()
            ),
            CST_REDIRECT_IN);
}

static Parser redirect_out_parser()
{
    return typed_parser(
            parser_sequence(2,
                        gen_state_parser_right_angle_bracket(),
                        name_parser()
            ),
            CST_REDIRECT_OUT);
}

static Parser cmd_sep_parser()
{
    return typed_parser(
            parser_choice(3,
                        pipe_parser(),
                        amp_parser(),
                        new_cmd_parser()
            ),
            CST_CMD_SEP);
}

static Parser pipe_parser()
{
    return typed_parser(
            gen_state_parser_pipe(),
            CST_PIPE);
}

static Parser amp_parser()
{
    return typed_parser(
            gen_state_parser_amp(),
            CST_AMP);
}

static Parser new_cmd_parser()
{
    return typed_parser(
            parser_choice(2,
                        gen_string_and_state_parser_semicolon(),
                        gen_string_and_state_parser_new_line()
            ),
            CST_NEW_CMD);
}

static bool literal_is_not_reserved(char * literal)
{
    return strcmp (literal, "!") != 0 && \
            strcmp(literal, "{") != 0 && \
            strcmp(literal, "}") != 0 && \
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
            strcmp(literal, "while") != 0;
}

static bool literal_parser_parser(ParseContext * ctx, Parser * p)
{
    (void)p;
    Token token = ctx->tokens[ctx->pos++];
    return token.state == STATE_LITERAL && literal_is_not_reserved(token.str);
}

static Parser literal_parser()
{
    return typed_parser(
            create_parser(literal_parser_parser, parser_commit_single_token),
            CST_LITERAL);
}

static Parser doubleq_parser()
{
    return typed_parser(
            gen_state_parser_doubleq(),
            CST_DOUBLEQ);
}

static Parser singleq_parser()
{
    return typed_parser(
            gen_state_parser_singleq(),
            CST_SINGLEQ);
}

static Parser optional_new_line_parser()
{
    return parser_optional(gen_string_and_state_parser_new_line());
}

static Parser if_condition_action()
{
    return typed_parser(
            parser_sequence(6,
                        names_parser(),
                        new_cmd_parser(),
                        gen_string_parser_then(),
                        optional_new_line_parser(),
                        forward_ref_parser(shell_instruction_parser),
                        new_cmd_parser()
            ),
            CST_IF_CONDITION_ACTION);
}

static Parser if_alternative()
{
    return typed_parser(
            parser_choice(2,
                        parser_sequence(4,
                                gen_string_parser_else(),
                                gen_string_and_state_parser_new_line(),
                                forward_ref_parser(shell_instruction_parser),
                                new_cmd_parser()
                        ),
                        parser_sequence(2,
                                gen_string_parser_elif(),
                                if_condition_action()
                        )
            ),
            CST_IF_ALTERNATIVE);
}

static Parser if_statement_parser()
{
    return typed_parser(
            parser_sequence(4,
                        gen_string_parser_if(),
                        if_condition_action(),
                        parser_repetition(if_alternative()),
                        gen_string_parser_fi()
            ),
            CST_IF_STATEMENT);
}

static Parser loop_body_parser()
{
    return typed_parser(
            parser_sequence(6,
                        new_cmd_parser(),
                        gen_string_parser_do(),
                        optional_new_line_parser(),
                        forward_ref_parser(shell_instruction_parser),
                        new_cmd_parser(),
                        gen_string_parser_done()
            ),
            CST_LOOP_BODY);
}

static Parser for_loop_parser()
{
    return typed_parser(
            parser_sequence(4,
                        gen_string_parser_for(),
                        literal_parser(),
                        parser_optional(
                                parser_sequence(2,
                                        gen_string_parser_in(),
                                        names_parser()
                                )
                        ),
                        loop_body_parser()
            ),
            CST_FOR_LOOP);
}

static Parser while_loop_parser()
{
    return typed_parser(
            parser_sequence(3,
                        gen_string_parser_while(),
                        names_parser(),
                        loop_body_parser()
            ),
            CST_WHILE_LOOP);
}

static Parser until_loop_parser()
{
    return typed_parser(
            parser_sequence(3,
                        gen_string_parser_until(),
                        names_parser(),
                        loop_body_parser()
            ),
            CST_UNTIL_LOOP);
}

static Parser case_expr_parser()
{
    return typed_parser(
            parser_sequence(6,
                        optional_new_line_parser(),
                        parser_optional(gen_string_parser_opening_parenthesis()),
                        separated_parser(
                                name_parser(),
                                gen_string_parser_single_pipe()
                        ),
                        gen_string_parser_closing_parenthesis(),
                        optional_new_line_parser(),
                        forward_ref_parser(shell_instruction_parser)
            ),
            CST_CASE_EXPR);
}

static Parser case_statement_parser()
{
    return typed_parser(
            parser_sequence(5,
                        gen_string_parser_case(),
                        literal_parser(),
                        gen_string_parser_in(),
                        parser_optional(
                                parser_choice(2,
                                        parser_sequence(2,
                                                separated_parser(
                                                        case_expr_parser(),
                                                        gen_string_and_state_parser_double_semicolon()
                                                ),
                                                parser_choice(2,
                                                      parser_sequence(3,
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
                        gen_string_parser_esac()
            ),
            CST_CASE_STATEMENT);
}
