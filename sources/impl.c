//
// Created on 17/05/2022.
//

#include "impl.h"

Parser command_all_parser()
{
    return typed_parser(
            parser_sequence(2,
                    command_parser(),
                    command_tail_parser()
                    ),
            COMMAND_ALL);
}

Parser command_parser()
{
    return typed_parser(
            parser_sequence(2,
                    names_parser(),
                    parser_optional(
                            redirect_parser()
                    )),
            COMMAND);
}

Parser command_tail_parser()
{
    return typed_parser(
            parser_repetition(
                    parser_sequence(2,
                                    cmd_sep_parser(),
                                    command_parser()
                    )),
            COMMAND_TAIL);
}

Parser name_parser()
{
    return typed_parser(
            parser_choice(3,
                          literal_parser(),
                          doubleq_parser(),
                          singleq_parser()),
            NAME);
}

Parser names_parser()
{
    return typed_parser(
            parser_sequence(2,
                            name_parser(),
                            parser_repetition(
                                    name_parser()
                            )),
            NAMES);
}

Parser redirect_parser()
{
    return typed_parser(
            parser_choice(2,
                    redirect_in_parser(),
                    redirect_out_parser()
                    ),
            REDIRECT);
}

Parser redirect_in_parser()
{
    return typed_parser(
            parser_sequence(2,
                    redirect_in_simple_parser(),
                    parser_optional(
                            redirect_out_simple_parser()
                    )
            ),
            REDIRECT_IN);
}

static bool left_angle_bracket_parser_parser(ParseContext * ctx, Parser * p)
{
    Token token = ctx->tokens[ctx->pos++];
    return token.state == STATE_LEFT_ANGLE_BRACKET;
}

Parser redirect_in_simple_parser()
{
    Parser left_angle_bracket_parser = create_parser(
            left_angle_bracket_parser_parser,
            parser_commit_single_token
    );
    return parser_sequence(2,
                        left_angle_bracket_parser,
                        literal_parser()
            );
}

Parser redirect_out_parser()
{
    return typed_parser(
            parser_sequence(2,
                    redirect_out_simple_parser(),
                    parser_optional(
                            redirect_in_simple_parser()
                    )
            ),
            REDIRECT_OUT);
}

static bool right_angle_bracket_parser_parser(ParseContext * ctx, struct Parser * p)
{
    Token token = ctx->tokens[ctx->pos++];
    return token.state == STATE_RIGHT_ANGLE_BRACKET;
}

Parser redirect_out_simple_parser()
{
    Parser right_angle_bracket_parser = create_parser(
            right_angle_bracket_parser_parser,
            parser_commit_single_token
    );
    return parser_sequence(2,
                        right_angle_bracket_parser,
                        literal_parser()
            );
}

Parser cmd_sep_parser()
{
    return typed_parser(
            parser_choice(3,
                          pipe_parser(),
                          amp_parser(),
                          new_cmd_parser()
            ),
            CMD_SEP);
}

static bool pipe_parser_parser(ParseContext * ctx, Parser * p)
{
    Token token = ctx->tokens[ctx->pos++];
    return token.state == STATE_PIPE;
}

Parser pipe_parser()
{
    return typed_parser(
            create_parser(pipe_parser_parser, parser_commit_single_token),
            PIPE);
}

static bool amp_parser_parser(ParseContext * ctx, Parser * p)
{
    Token token = ctx->tokens[ctx->pos++];
    return token.state == STATE_AMPERSAND;
}

Parser amp_parser()
{
    return typed_parser(
            create_parser(amp_parser_parser, parser_commit_single_token),
            AMP);
}

static bool semicolon_parser(ParseContext * ctx, Parser * p)
{
    Token token = ctx->tokens[ctx->pos++];
    return token.str_len == 1 && token.str[0] == ';' && token.str[1] == 0x0;
}

static bool newline_parser(ParseContext * ctx, Parser * p)
{
    Token token = ctx->tokens[ctx->pos++];
    return token.str_len == 1 && token.str[0] == '\n' && token.str[1] == 0x0;
}

Parser new_cmd_parser()
{
    Parser semicolon_p = create_parser(semicolon_parser, parser_commit_single_token);
    Parser newline_p = create_parser(newline_parser, parser_commit_single_token);
    return typed_parser(
            parser_choice(2,
                          semicolon_p,
                          newline_p
            ),
            NEW_CMD);
}

static bool literal_parser_parser(ParseContext * ctx, Parser * p)
{
    Token token = ctx->tokens[ctx->pos++];
    return token.state == STATE_LITERAL;
}

Parser literal_parser()
{
    return typed_parser(
            create_parser(literal_parser_parser, parser_commit_single_token),
            LITERAL);
}

static bool doubleq_parser_parser(ParseContext * ctx, Parser * p)
{
    Token token = ctx->tokens[ctx->pos++];
    return token.state == STATE_DOUBLE_QUOTES;
}

Parser doubleq_parser()
{
    return typed_parser(
            create_parser(doubleq_parser_parser, parser_commit_single_token),
            DOUBLEQ);
}

static bool singleq_parser_parser(ParseContext * ctx, Parser * p)
{
    Token token = ctx->tokens[ctx->pos++];
    return token.state == STATE_SINGLE_QUOTES;
}

Parser singleq_parser()
{
    return typed_parser(
            create_parser(singleq_parser_parser, parser_commit_single_token),
            SINGLEQ);
}
