#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "lexer/token.h"
#include "parser/ast.h"
#include "impl.h"
#include "test.h"
#include "interactive.h"
#include "lexer/shell_grammar/lexical_conventions.h"

#define FILE_READ_BUFFER_SIZE 2048
#define USAGE_EXIT \
{ \
    fprintf(stderr, "Usage: './Ocean -f filename', './Ocean -t test-name flags' or './Ocean -i flags'\n"); \
    exit(1); \
}

#define FILE_MODE           0b00001
#define TEST_MODE           0b00010
#define INTERACTIVE_MODE    0b00100

static char * read_file(char * filename);

static void traverse_cst(CSTNode cst, int depth);
static void traverse_ast(ASTNode ast, int depth);

int main(int argc, char ** argv) {
    // read file from cmd arg
    if (argc < 2 || argc > 4)
        USAGE_EXIT
    if (strlen(argv[1]) != 2)
        USAGE_EXIT
    char program_mode_char = argv[1][1];
    char * content = NULL;
    unsigned int program_mode = 0;
    if (program_mode_char == 'f') {
        program_mode = FILE_MODE;
        assert(argc == 3); // -f filename
        char * filename = argv[2];
        content = read_file(filename);
    } else if (program_mode_char == 't') {
        program_mode = TEST_MODE;
        assert(argc == 4); // -t test-name flags
        char * test_name = argv[2];
        char * end_ptr = NULL;
        long flags = strtol(argv[3], &end_ptr, 0);
        assert(end_ptr - argv[3] == strlen(argv[3])); // make sure the whole argument is a number
        size_t test_name_len = strlen(test_name),
               filename_len = 6 + test_name_len + 6;
        char * input_filename = malloc(filename_len + 1); // tests/ + test_name + /input
        char * output_filename = malloc(filename_len + 2); // tests/ + test_name + /output
        memcpy(input_filename, "tests/", 6);
        memcpy(input_filename + 6, test_name, test_name_len);
        // test root
        memcpy(output_filename, input_filename, 6 + test_name_len);
        strcpy(input_filename + 6 + test_name_len, "/input");
        strcpy(output_filename + 6 + test_name_len, "/output");
        // read files
        chdir("..");
        char * test_input = read_file(input_filename);
        char * test_output = read_file(output_filename);
        // free filenames
        free(input_filename);
        free(output_filename);

        // launch test
        start_test(flags, test_input, test_output);
    } else if (program_mode_char == 'i') {
        assert(argc == 3);
        char * end_ptr = NULL;
        long flags = strtol(argv[2], &end_ptr, 0);
        assert(end_ptr - argv[2] == strlen(argv[2])); // make sure the whole argument is a number
        interactive_mode(flags);
    }

    // tokenize content
    size_t num_tokens = 0;
    Token * tokens = tokenize(content, &num_tokens);
    free(content);

    // apply lexical conventions
    lexical_conventions_rules(tokens, num_tokens);

#ifdef OCEAN_DEBUG_TOKENS_MACRO
    for (size_t i = 0; i < num_tokens; i++)
        printf("Token (%zu) %s : '%s'\n", tokens[i].str_len, TOKEN_TYPE_STRING(tokens[i].type), tokens[i].str);
    exit(0);
#endif

    // exec tokens
    ParseContext ctx = create_parse_ctx(tokens, num_tokens);
    Combinator program_parser_p = program_parser();

    bool success = program_parser_p.exec(&ctx, &program_parser_p);
    if (!success || ctx.pos != ctx.num_tokens)
        fprintf(stderr, "Could not consume all tokens: %d out of %zu\n", ctx.pos, ctx.num_tokens);

    // prune_cst(&ctx.cst);
    traverse_cst(ctx.cst, 0);

    /*
    ASTNode ast = abstract_cst(ctx.cst);
    traverse_ast(ast, 0);
    */

    free_cst_node_children(ctx.cst);
    free(tokens);

    return 0;
}

static void print_cst_node(CSTNode node, int depth)
{
    for (int i = 0; i < depth; i++)
        putchar('\t');
    printf("cst_%s : %s\n",
           PARSER_TYPE_STRING(node.type),
           node.token == NULL ? "" : (
                   node.token->str[0] == '\n'
                        ? "\\n"
                        : node.token->str
                   ));
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
void traverse_cst(CSTNode cst, int depth)
{
    print_cst_node(cst, depth++);
    for (size_t i = 0; i < cst.num_children; i++)
        traverse_cst(*cst.children[i], depth);
}

/*
static void print_ast_node(ASTNode node, int depth)
{
    for (int i = 0; i < depth; i++)
        putchar('\t');
    printf("ast_%s : %s\n",
           ABSTRACT_NODE_TYPE_STRING[node.type],
           node.str == NULL ? "" : node.str
           );
}

void traverse_ast(ASTNode ast, int depth)
{
    print_ast_node(ast, depth++);
    for (size_t i = 0; i < ast.num_children; i++)
        traverse_ast(ast.children[i], depth);
}
*/

char * read_file(char * filename) {
    // Open file
    int input_file = open(filename, O_RDONLY, 0444);
    if (input_file == -1) {
        fprintf(stderr, "Cannot open input file\n");
        exit(1);
    }
    // Alloc memory to read file
    size_t content_size = FILE_READ_BUFFER_SIZE;
    char * content = malloc(content_size);
    ssize_t cursor, num_chars_read = 0;
    while ((cursor = read(input_file, content + content_size - FILE_READ_BUFFER_SIZE, FILE_READ_BUFFER_SIZE)) != 0) {
        if (cursor == -1) {
            fprintf(stderr, "Cannot read input file\n");
            free(content);
            close(input_file);
            exit(1);
        }
        num_chars_read += cursor;
        if (content_size - num_chars_read < FILE_READ_BUFFER_SIZE)
            content = realloc(content, content_size += FILE_READ_BUFFER_SIZE);
    }
    // chars read fill up entirely the content buffer (very rare tho)
    if (num_chars_read == content_size)
        content = realloc(content, ++content_size);
    // terminate content
    content[num_chars_read] = 0x0;
    return content;
}

#pragma clang diagnostic pop
