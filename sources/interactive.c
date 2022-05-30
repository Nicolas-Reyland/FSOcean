//
// Created by Nicolas Reyland on 5/27/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "interactive.h"
#include "lexer/token.h"
#include "string_utils/string_utils.h"
#include "lexer/shell_grammar/lexical_conventions.h"

#define MAX_LINE_LENGTH 256
#define FLAG_LITERAL_LENGTH 10

static char FLAGS[][FLAG_LITERAL_LENGTH] = {
        {'T', 'L', 'C', 0, 1}, // Tokens Lexical Conventions
};

static char * get_flag(char * key);
static void switch_flag(char * key, int value);
static void info_flag(char * key, size_t line_size);

static void interactive_tokens_mode(long flags);

static Token * tokenize_with_flags(char buffer[256], size_t * num_tokens);

void interactive_mode(long flags) {
    if (flags & INTERACTIVE_TOKENS) {
        interactive_tokens_mode(flags);
    }
    fprintf(stderr, "Unknown mode: %ld\n", flags);
    exit(1);
}

void interactive_tokens_mode(long flags) {
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
        if (line_size > 2 && str_is_prefix(line_buffer, "##")) {
            char command_char = line_buffer[2];
            switch (command_char) {
                case '?':
                    info_flag(line_buffer + 4, line_size);
                    break;
                case '+':
                    if (line_size < 5) {
                        fprintf(stderr, "Usage: ##%c <flag-name>\n", command_char);
                        break;
                    }
                    switch_flag(line_buffer + 4, 1);
                    break;
                case '-':
                    if (line_size < 5) {
                        fprintf(stderr, "Usage: ##%c <flag-name>\n", command_char);
                        break;
                    }
                    switch_flag(line_buffer + 4, 0);
                    break;
                case 's':
                    if (line_size < 5) {
                        fprintf(stderr, "Usage: ##%c <flag-name>\n", command_char);
                        break;
                    }
                    switch_flag(line_buffer + 4, -1);
                    break;
                default:
                    fprintf(stderr, "Unknown command '%c'\n", command_char);
                    break;
            }
            printf(" (tokens) $ ");
            fflush(stdout);
            continue;
        }
        offset = 0;
        size_t num_tokens;
        Token * tokens = tokenize_with_flags(line_buffer, &num_tokens);
        print_tokens(tokens, num_tokens);
        printf(" (tokens) > ");
        fflush(stdout);
    }
    exit(0);
}

static Token * tokenize_with_flags(char buffer[MAX_LINE_LENGTH], size_t * num_tokens)
{
    Token * tokens = tokenize(buffer, num_tokens);
    if (get_flag("TLC")[4]) {
        lexical_conventions_rules(tokens, *num_tokens);
    }

    return tokens;
}

static char * get_flag(char * key)
{
    size_t key_size = strlen(key);
    if (key[key_size - 1] == '\n')
        key[key_size - 1] = 0;
    size_t num_flags = sizeof(FLAGS) / FLAG_LITERAL_LENGTH;
    for (size_t i = 0; i < num_flags; i++) {
        if (strcmp(FLAGS[i], key) == 0)
            return FLAGS[i];
    }
    fprintf(stderr, "Cannot find flag: %s\n", key);
    return NULL;
}

static void info_flag(char * key, size_t line_size) {
    if (line_size < 5) {
        size_t num_flags = sizeof(FLAGS) / FLAG_LITERAL_LENGTH;
        for (size_t i = 0; i < num_flags; i++) {
            size_t flag_size = strlen(FLAGS[i]);
            printf(" - Flag %s: %d\n", FLAGS[i], FLAGS[i][flag_size + 1]);
        }
        return;
    } else {
        char * flag = get_flag(key);
        if (flag == NULL)
            fprintf(stderr, "Flag %s not found\n", key);
        size_t flag_size = strlen(flag);
        printf(" * Flag %s: %d\n", flag, flag[flag_size + 1]);
    }
}

static void switch_flag(char * key, int value) {
    char * flag = get_flag(key);
    size_t key_size = strlen(key);
    assert(flag[key_size + 1] == 0 || flag[key_size + 1] == 1);
    if (value == -1)
        value = !flag[key_size + 1];
    printf("Switching flag %s to %d\n", key, value);
    assert(value == 0 || value == 1);
    flag[key_size + 1] = (char)value;
}
