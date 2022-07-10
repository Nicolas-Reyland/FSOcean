//
// Created by Nicolas Reyland on 5/26/22.
//

#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "testing/test.h"
#include "lexer/token.h"
#include "misc/output.h"
#include "parser/parse_context.h"
#include "misc/impl.h"
#include "lexer/shell_grammar/lexical_conventions.h"

#define CONTENT_BUFFER_SIZE 256

#define TEST_TOKENS             0x00001
#define TEST_TOKENS_TLC         0x00002

#define TEST_PARSERS            0x00010
#define TEST_PARSERS_SHOW_TK    0x00002

#define TEST_ABSTRACT           0x00100

#define TEST_EXECUTION          0x01000

static void start_test_tokens(long flags, const char * input, size_t input_len);
static void start_test_parsers(long flags, const char * input, size_t input_len);

noreturn void start_test(long flags, char * input, size_t input_len, char * output, size_t output_len) {
    // create a pipe to read from stdout (yes, stdout)
    int stdout_bk = dup(fileno(stdout)); // fd for stdout backup
    int fds[2];
    pipe2(fds, 0); // O_NONBLOCK
    dup2(fds[1], fileno(stdout));
    // start the test
    if (flags & TEST_TOKENS) {
        start_test_tokens(flags, input, input_len);
        fflush(NULL);
    } else if (flags & TEST_PARSERS) {
        start_test_parsers(flags, input, input_len);
        fflush(NULL);
    }
    // read the test output
    char buf[CONTENT_BUFFER_SIZE];
    ssize_t cursor;
    size_t output_size = strlen(output),
           num_read = 0;
    while ((cursor = read(fds[0], buf, CONTENT_BUFFER_SIZE)) != 0) {
        bool theory_output_too_small = num_read + cursor > output_size;
        if (theory_output_too_small || memcmp(buf, output + num_read, cursor) != 0) {
            // terminate theory output at chunk and print
            if (!theory_output_too_small)
                output[num_read + cursor] = 0;
            // terminate practice output at chunk and show diff
            buf[cursor] = 0;
            show_output_diff(output + num_read, buf, cursor);
            // restore stdout
            dup2(stdout_bk, fileno(stdout));
            // flush all and exit
            fflush(NULL);
            printf("FAILED");
            exit(1);
        }
        num_read += cursor;
        if (cursor != CONTENT_BUFFER_SIZE) {
            break;
        }
    }
    // restore stdout and flush it
    dup2(stdout_bk, fileno(stdout));
    fflush(NULL);
    // free stuff and exit
    free(input);
    free(output);
    printf("SUCCESS");
    exit(0);
}

static void start_test_tokens(long flags, const char * input, size_t input_len)
{
    // tokenize content
    size_t num_tokens = 0;
    Token * tokens = tokenize(input, input_len, &num_tokens);
    if (flags & TEST_TOKENS_TLC)
        lexical_conventions_rules(tokens, num_tokens);
    print_tokens(tokens, num_tokens);
}

static void start_test_parsers(long flags, const char * input, size_t input_len)
{
    // tokenize content
    size_t num_tokens = 0;
    Token * tokens = tokenize(input, input_len, &num_tokens);
    lexical_conventions_rules(tokens, num_tokens);
    if (flags & TEST_PARSERS_SHOW_TK)
        print_tokens(tokens, num_tokens);
    // parse content
    ParseContext ctx = create_parse_ctx(tokens, num_tokens);
    Parser parser = program_parser();
    bool success = parser.exec(&ctx, &parser);
    printf("Parsing result : %s\n", success ? "SUCCESS" : "FAILED");
    traverse_cst(ctx.cst, 0);
}
