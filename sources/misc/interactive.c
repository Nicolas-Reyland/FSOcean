//
// Created by Nicolas Reyland on 5/27/22.
//

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "misc/interactive.h"
#include "lexer/token.h"
#include "string_utils/string_utils.h"
#include "lexer/shell_grammar/lexical_conventions.h"
#include "misc/output.h"
#include "parser/parse_context.h"
#include "misc/impl.h"
#include "misc/safemem.h"
#include "executable/executable.h"
#include "parser/abstractions/abstraction.h"

#define CUSTOM_FLAGS_HANDLING(prompt_text) \
    if (line_len > 2 && str_is_prefix(line_buffer, "##")) { \
        bool break_from_loop = false; \
        char command_char = line_buffer[2]; \
        switch (command_char) { \
            case 'h': { \
                printf(" Help on special commands:\n" \
                "  - ##h : show this help\n" \
                "  - ##? : list all flags\n" \
                "  - ##+ <flag> : set <flag> to 1\n" \
                "  - ##+ <flag> : set <flag> to 0\n" \
                "  - ##s <flag> : switch <flag>\n" \
                "  - ##q : quit the program\n" \
                "\n"); \
                } break; \
            case '?': { \
                info_flag(line_buffer + 4, line_len); \
                } break; \
            case '+': { \
                if (line_len < 5) { \
                    print_error(OCERR_EXIT, "Usage: ##%c <flag-name>\n", command_char); \
                    break; \
                } \
                switch_flag(line_buffer + 4, 1); \
                } break; \
            case '-': { \
                if (line_len < 5) { \
                    print_error(OCERR_EXIT, "Usage: ##%c <flag-name>\n", command_char); \
                    break; \
                } \
                switch_flag(line_buffer + 4, 0); \
                } break; \
            case 's': { \
                if (line_len < 5) { \
                    print_error(OCERR_EXIT, "Usage: ##%c <flag-name>\n", command_char); \
                    break; \
                } \
                switch_flag(line_buffer + 4, -1); \
                } break; \
            case 'q': { \
                break_from_loop = true; \
            } break; \
            default: { \
                print_error(OCERR_EXIT, "Unknown command '%c'\n", command_char); \
                } break; \
        } \
        printf(" (" #prompt_text ") $ "); \
        fflush(stdout); \
        if (break_from_loop) { \
            break; \
        } \
        continue; \
    }

#define MAX_LINE_LENGTH 256
#define FLAG_LITERAL_LENGTH 10

static char FLAGS[][FLAG_LITERAL_LENGTH] = {
        {'T', 'L', 'C', 0, 1}, // Tokens Lexical Conventions
        {'S', 'G', 'R', 0, 1}, // Shell Grammar Rules
        {'A', 'E', 'X', 'E', 0, 1}, // Abstract Executables
};

static char * get_flag(char * key);
static void switch_flag(char * key, int value);
static void info_flag(char * key, size_t line_size);

static noreturn void interactive_tokens_mode(long flags);
static noreturn void interactive_cst_mode(long flags);

static Token * tokenize_with_flags(char buffer[256], size_t line_len, size_t * num_tokens);

void interactive_mode(long flags) {
    if (flags & INTERACTIVE_TOKENS) {
        interactive_tokens_mode(flags);
    }
    if (flags & INTERACTIVE_CST) {
        interactive_cst_mode(flags);
    }
    print_error(OCERR_EXIT, "Unknown mode: %ld\n", flags);
    exit(1);
}

noreturn void interactive_tokens_mode(long flags) {
    (void)flags;
    char line_buffer[MAX_LINE_LENGTH];
    printf("- INTERACTIVE MODE -\n Try '##h' for help on commands.\n\n");
    printf(" (tokens) $ ");
    fflush(stdout);
    size_t offset = 0;
    while (fgets(line_buffer + offset, MAX_LINE_LENGTH, stdin) != NULL) {
        size_t line_len = strlen(line_buffer);
        if (line_len > 1 && line_buffer[line_len - 2] == '\\') {
            offset = line_len - 2;
            printf(" (tk) > ");
            fflush(stdout);
            continue;
        }
        CUSTOM_FLAGS_HANDLING(tokens)
        offset = 0;
        size_t num_tokens;
        Token * tokens = tokenize_with_flags(line_buffer, line_len, &num_tokens);
        print_tokens(tokens, num_tokens);
        printf(" (tokens) > ");
        fflush(stdout);
    }

    printf("\nQuit. Freeing memory.\n");
    free_all_registered_memory();
    exit(0);
}

static Token * tokenize_with_flags(char buffer[MAX_LINE_LENGTH], size_t line_len, size_t * num_tokens)
{
    Token * tokens = tokenize(buffer, line_len, num_tokens);
    if (get_flag("TLC")[4]) {
        lexical_conventions_rules(tokens, *num_tokens);
        if (get_flag("SGR")[4]) {
            // shell_grammar_rules(tokens, *num_tokens);
        }
    }

    return tokens;
}

static noreturn void interactive_cst_mode(long flags)
{
    (void)flags;
    Parser program_parser_p = program_parser();
    char line_buffer[MAX_LINE_LENGTH];
    printf("- INTERACTIVE MODE -\n Try '##h' for help on commands.\n\n");
    printf(" (cst) $ ");
    fflush(stdout);
    size_t offset = 0;
    while (fgets(line_buffer + offset, MAX_LINE_LENGTH, stdin) != NULL) {
        size_t line_len = strlen(line_buffer);
        if (line_len > 1 && line_buffer[line_len - 2] == '\\') {
            // overwrite escaped new-line with actual new-line
            line_buffer[line_len - 2] = '\n';
            offset = line_len - 1;
            printf(" (cst) > ");
            fflush(stdout);
            continue;
        }
        CUSTOM_FLAGS_HANDLING(cst)
        offset = 0;
        size_t num_tokens;
        Token * tokens = tokenize_with_flags(line_buffer, line_len, &num_tokens);
        ParseContext ctx = create_parse_ctx(tokens, num_tokens);
        bool success = program_parser_p.exec(&ctx, &program_parser_p);
        traverse_cst(ctx.cst, 0);
        if (get_flag("AEXE")) {
            Executable executable = abstract_cst(ctx.cst);
            traverse_executable(executable, 0);
        }
        printf(" (cst) > ");
        fflush(stdout);
    }

    printf("\nQuit. Freeing memory.\n");
    free_all_registered_memory();
    exit(0);
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
    print_error(OCERR_EXIT, "Cannot find flag: %s\n", key);
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
            print_error(OCERR_EXIT, "Flag %s not found\n", key);
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
