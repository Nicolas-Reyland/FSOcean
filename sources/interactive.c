//
// Created by Nicolas Reyland on 5/27/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interactive.h"
#include "lexer/token.h"

#define MAX_LINE_LENGTH 256

static void interactive_tokens_mode(long flags);

void interactive_mode(long flags) {
    if (flags & INTERACTIVE_TOKENS) {
        interactive_tokens_mode(flags);
    }
    fprintf(stderr, "Unknown mode: %ld\n", flags);
    exit(1);
}

noreturn void interactive_tokens_mode(long flags) {
    (void)flags;
    char line_buffer[MAX_LINE_LENGTH];
    printf(" (tokens) $ ");
    fflush(stdout);
    size_t offset = 0;
    while (fgets(line_buffer + offset, MAX_LINE_LENGTH, stdin) != NULL) {
        size_t line_size = strlen(line_buffer);
        if (line_size > 1 && line_buffer[line_size - 2] == '\\') {
            offset = line_size - 2;
            printf(" (tk) > ");
            fflush(stdout);
            continue;
        }
        offset = 0;
        size_t num_tokens;
        Token * tokens = tokenize(line_buffer, &num_tokens);
        print_tokens(tokens, num_tokens);
        printf(" (tokens) > ");
        fflush(stdout);
    }
    exit(0);
}
