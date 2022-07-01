//
// Created by Nicolas Reyland on 5/26/22.
//

#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <fcntl.h>              /* Obtain O_* constant definitions */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "test.h"
#include "lexer/token.h"

#define CONTENT_BUFFER_SIZE 256

#define TEST_TOKENS         0b00001
#define TEST_PARSERS        0b00010
#define TEST_ABSTRACT       0b00100
#define TEST_EXECUTION      0b01000

static void start_test_tokens(long flags, const char * input, size_t input_len);
static void show_output_diff(const char * theory, char * practice, size_t content_len);

noreturn void start_test(long flags, char * input, size_t input_len, char * output, size_t output_len) {
    // create a pipe to read from stdout (yes, stdout)
    int stdout_bk = dup(fileno(stdout)); // fd for stdout backup
    int fds[2];
    pipe2(fds, 0); // O_NONBLOCK
    dup2(fds[1], fileno(stdout));
    // start the test
    if (flags && TEST_TOKENS) {
        start_test_tokens(flags, input, input_len);
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
    exit(0);
}

static void start_test_tokens(long flags, const char * input, size_t input_len)
{
    // tokenize content
    size_t num_tokens = 0;
    Token * tokens = tokenize(input, input_len, &num_tokens);
    print_tokens(tokens, num_tokens);
}

static void show_output_diff(const char * theory, char * practice, size_t content_len)
{
    fprintf(stderr, "test failed :\n\n - THEORY -\n%s\n\n", theory);
    fprintf(stderr, " - PRACTICE -\n");
    size_t diff_c_index = 0;
    for (; diff_c_index < content_len && theory[diff_c_index] == practice[diff_c_index]; diff_c_index++);
    assert(diff_c_index != content_len);
    char diff_c = practice[diff_c_index];
    practice[diff_c_index] = 0x0;
    fprintf(stderr, "%s>>> %c <<<%s\n", practice, diff_c, practice + diff_c_index + 1);
}
