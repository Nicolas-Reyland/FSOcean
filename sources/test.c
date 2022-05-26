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
#include "test.h"
#include "lexer/token.h"

#define CONTENT_BUFFER_SIZE 256

#define TEST_TOKENS         0b00001
#define TEST_PARSERS        0b00010
#define TEST_ABSTRACT       0b00100
#define TEST_EXECUTION      0b01000

static void start_test_tokens(long flags, const char * input);

noreturn void start_test(long flags, char * input, char * output) {
    // create a pipe to read from stdout (yes, stdout)
    int stdout_bk = dup(fileno(stdout)); // fd for stdout backup
    int fds[2];
    pipe2(fds, 0); // O_NONBLOCK
    dup2(fds[1], fileno(stdout));
    // start the test
    if (flags && TEST_TOKENS) {
        start_test_tokens(flags, input);
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
            fprintf(stderr, "test failed :\n\n");
            // terminate theory output at chunk and print
            if (!theory_output_too_small)
                output[num_read + cursor] = 0;
            fprintf(stderr, " - THEORY -\n%s\n\n", output + num_read);
            // terminate practice output at chunk and print
            buf[cursor] = 0;
            fprintf(stderr, " - PRACTICE -\n%s\n", buf);
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

static void start_test_tokens(long flags, const char * input)
{
    // tokenize content
    size_t num_tokens = 0;
    Token * tokens = tokenize(input, &num_tokens);
    for (size_t i = 0; i < num_tokens; i++) {
        if (tokens[i].type == OPERATOR_TOKEN && tokens[i].str_len == 1 && tokens[i].str[0] == '\n')
            printf(".\n");
        else
            printf("T: (%s) '%s'\n", ATOM_TYPE_STRING(tokens[i].type), tokens[i].str);
    }
}
