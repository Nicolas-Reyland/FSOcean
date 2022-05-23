#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <fcntl.h>
#include "lexer/token.h"
#include "combinators/combinator.h"
#include "impl.h"
#include "parser/ast.h"

#define FILE_CONTENT_BUFFER_SIZE 1048

void traverse_cst(CSTNode cst, int depth);
void traverse_ast(ASTNode ast, int depth);

int main(int argc, char ** argv) {
    // read file from cmd arg
    if (argc != 2) {
        fprintf(stderr, "need exactly one command-line argument: filename\n");
        exit(1);
    }
    char * filename = argv[1];
    int input_file = open(filename, O_RDONLY, 0444);
    if (input_file == -1) {
        fprintf(stderr, "Cannot open input file\n");
        exit(1);
    }
    char file_content[FILE_CONTENT_BUFFER_SIZE] = {};
    ssize_t cursor = read(input_file, file_content, FILE_CONTENT_BUFFER_SIZE);
    if (cursor == -1) {
        fprintf(stderr, "Cannot read input file\n");
        close(input_file);
        exit(1);
    }
    if (cursor == FILE_CONTENT_BUFFER_SIZE) {
        fprintf(stderr, "Input file is too big\n");
        close(input_file);
        exit(1);
    }
    file_content[cursor] = 0x0;
    // tokenize content
    size_t num_tokens = 0;
    Token * tokens = tokenize(file_content, &num_tokens);
    // strip whitespace tokens
    tokens = strip_tokens(tokens, &num_tokens);

#ifdef OCEAN_DEBUG_TOKENS_MACRO
    for (size_t i = 0; i < num_tokens; i++)
        printf("Token (%zu) %s : '%s'\n", tokens[i].str_len, STATE_STRING(tokens[i].state), tokens[i].str);
    exit(0);
#endif

    // exec tokens
    ParseContext ctx = create_parse_ctx(tokens, num_tokens);
    Combinator shell_instruction_p = shell_instruction_parser();

    bool success = shell_instruction_p.exec(&ctx, &shell_instruction_p);
    if (!success || ctx.pos != ctx.num_tokens)
        fprintf(stderr, "Could not consume all tokens: %d out of %zu\n", ctx.pos, ctx.num_tokens);

    // prune_cst(&ctx.cst);
    traverse_cst(ctx.cst, 0);

    ASTNode ast = abstract_cst(ctx.cst);
    traverse_ast(ast, 0);

    free_cst_node_children(ctx.cst);
    free(tokens);

    return 0;
}

static void print_cst_node(CSTNode node, int depth)
{
    for (int i = 0; i < depth; i++)
        putchar('\t');
    printf("cst_%s : %s\n",
           CONCRETE_NODE_TYPE_STRING[node.type],
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
#pragma clang diagnostic pop
