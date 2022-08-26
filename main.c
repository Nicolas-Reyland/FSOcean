#include <stdio.h>
#include <stddef.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "lexer/token.h"
#include "parser/ast.h"
#include "misc/impl.h"
#include "testing/test.h"
#include "misc/interactive.h"
#include "lexer/shell_grammar/lexical_conventions.h"
#include "misc/output.h"
#include "misc/safemem.h"

#define FILE_READ_BUFFER_SIZE 2048
#define USAGE_EXIT \
{ \
    print_error(OCERR_EXIT, "Usage: ./Ocean ['-f filename', '-t test-name flags', '-i flags', '-p \"command to parse\"', '-e \"command to execute\"']\n"); \
}

#define FILE_MODE           0b00001
#define TEST_MODE           0b00010
#define INTERACTIVE_MODE    0b00100

char * read_file(char * filename, size_t * content_len);

// static void traverse_ast(ASTNode ast, int depth);

int main(int argc, char ** argv) {
    // read file from cmd arg
    if (argc < 2 || argc > 4)
        USAGE_EXIT
    if (strlen(argv[1]) != 2)
        USAGE_EXIT
    // Program start
    init_memory_registration();
    char program_mode_char = argv[1][1];
    char * content = NULL;
    size_t content_len = 0;
    unsigned int program_mode = 0;
    switch (program_mode_char) {
        case 'f': {
            program_mode = FILE_MODE;
            assert(argc == 3); // -f filename
            char *filename = argv[2];
            content = read_file(filename, &content_len);
        } break;
        case 't': {
            program_mode = TEST_MODE;
            assert(argc == 4); // -t test-name flags
            char *test_name = argv[2];
            char *end_ptr = NULL;
            long flags = strtol(argv[3], &end_ptr, 0);
            assert(end_ptr - argv[3] == strlen(argv[3])); // make sure the whole argument is a number
            size_t test_name_len = strlen(test_name),
                    filename_len = 6 + test_name_len + 6;
            char *input_filename = reg_malloc(filename_len + 1); // tests/ + test_name + /input
            char *output_filename = reg_malloc(filename_len + 2); // tests/ + test_name + /output
            memcpy(input_filename, "tests/", 6);
            memcpy(input_filename + 6, test_name, test_name_len);
            // test root
            memcpy(output_filename, input_filename, 6 + test_name_len);
            strcpy(input_filename + 6 + test_name_len, "/input");
            strcpy(output_filename + 6 + test_name_len, "/output");
            // read files
            chdir("..");
            size_t test_input_len, test_output_len;
            char *test_input = read_file(input_filename, &test_input_len);
            char *test_output = read_file(output_filename, &test_output_len);
            // free filenames
            reg_free(input_filename);
            reg_free(output_filename);

            // launch test
            start_test(flags, test_input, test_input_len, test_output, test_output_len);
        };
        case 'i': {
            assert(argc == 3);
            char *end_ptr = NULL;
            long flags = strtol(argv[2], &end_ptr, 0);
            assert(end_ptr - argv[2] == strlen(argv[2])); // make sure the whole argument is a number
            interactive_mode(flags);
        };
        case 'p': {
            assert(argc == 3);
            content_len = strlen(argv[2]);
            content = reg_malloc(content_len + 1);
            strcpy(content, argv[2]);
        } break;
        case 'e': {
            print_error(OCERR_EXIT, "Not Implemented yet\n");
        };
        default:
            USAGE_EXIT
    }

    // tokenize content
    size_t num_tokens = 0;
    Token * tokens = tokenize(content, content_len, &num_tokens);
    reg_free(content);

    // apply lexical conventions
    lexical_conventions_rules(tokens, num_tokens);

#ifdef OCEAN_DEBUG_TOKENS_MACRO
    for (size_t i = 0; i < num_tokens; i++)
        printf("Token (%zu) %s : '%s'\n", tokens[i].str_len, TOKEN_TYPE_STRING(tokens[i].type), tokens[i].str);
    exit(0);
#endif

    // exec tokens
    ParseContext ctx = create_parse_ctx(tokens, num_tokens);
    Parser program_parser_p = program_parser();

    bool success = program_parser_p.exec(&ctx, &program_parser_p);

    traverse_cst(ctx.cst, 0);

    // Stderr output
    if (!success || ctx.pos != ctx.num_tokens - 1)
        print_error(OCERR_STDERR, "Could not consume all tokens: %d out of %zu\n", ctx.pos, ctx.num_tokens);
    else // yes, printing SUCCESS to stderr is not logical, but it's CLion's fault anyway (no)
        print_error(OCERR_STDERR,  "SUCCESS\n");

    // Abstract Parser tree
    Executable executable = abstract_cst(ctx.cst);

    // Execute executable
    // exec_executable(executable);

    // free the tokens
    free_tokens(tokens, num_tokens);
    free_tokens(ctx.tokens_backup, num_tokens);
    // free the context
    free_parser_ctx(ctx);
    // free the parsers
    free_parser(program_parser_p);
    // free the cst nodes
    free_cst_node_children(ctx.cst);

    // free the rest
    free_all_registered_memory();

    return 0;
}

char * read_file(char * filename, size_t * content_len) {
    // Open file
    int input_file = open(filename, O_RDONLY, 0444);
    if (input_file == -1) {
        print_error(OCERR_EXIT, "Cannot open file: '%s'\n", filename);
    }
    // Alloc memory to read file
    size_t content_size = FILE_READ_BUFFER_SIZE;
    char * content = reg_malloc(content_size);
    ssize_t cursor, num_chars_read = 0;
    while ((cursor = read(input_file, content + content_size - FILE_READ_BUFFER_SIZE, FILE_READ_BUFFER_SIZE)) != 0) {
        if (cursor == -1) {
            print_error(OCERR_EXIT, "Cannot read input file\n");
            reg_free(content);
            close(input_file);
            exit(1);
        }
        num_chars_read += cursor;
        if (content_size - num_chars_read < FILE_READ_BUFFER_SIZE)
            content = reg_realloc(content, content_size += FILE_READ_BUFFER_SIZE);
    }
    content = reg_realloc(content, num_chars_read + 1);
    // terminate content
    content[num_chars_read] = 0x0;
    if (content_len != NULL)
        *content_len = num_chars_read;
    return content;
}
