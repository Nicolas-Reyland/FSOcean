#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "lexer/token.h"
#include "parser/parser.h"
#include "impl.h"

#define FILE_CONTENT_BUFFER_SIZE 256

void traverse_cst(CSTNode cst, int depth);

int main(int argc, char** argv) {
    // read file from cmd arg
    if (argc != 2) {
        fprintf(stderr, "need exactly one command-line argument: filename\n");
        exit(1);
    }
    char* filename = argv[1];
    int input_file = open(filename, O_RDONLY, 0444);
    char file_content[FILE_CONTENT_BUFFER_SIZE] = {};
    ssize_t cursor = read(input_file, file_content, FILE_CONTENT_BUFFER_SIZE);
    if (cursor == FILE_CONTENT_BUFFER_SIZE) {
        fprintf(stderr, "File is too big\n");
        close(input_file);
        exit(1);
    }
    file_content[cursor] = 0x0;
    // tokenize content
    size_t num_tokens = 0;
    Token * tokens = tokenize(file_content, &num_tokens);
//    Token * tokens = tokenize("echo hi", &num_tokens);
    // strip whitespace tokens
    tokens = strip_tokens(tokens, &num_tokens);
    // parse tokens
    ParseContext ctx = create_parse_ctx(tokens, num_tokens);
    Parser command_p = command_parser();

    bool success = command_p.parse(&ctx, &command_p);
    if (!success) {
        fprintf(stderr, "Could not consume all tokens: %zu out of %zu\n", ctx.pos, ctx.num_tokens);
        exit(1);
    }

    /*
    for (size_t i = 0; i < num_tokens; i++) {
        if (!tokens[i].eof_or_empty)
            printf("Token[%zu] (%s): %s\n", i, STATE_STRING(tokens[i].state), tokens[i].str);
    }
    */

    traverse_cst(ctx.cst, 0);
    fflush(NULL);

    return 0;
}

static void print_cst_node(CSTNode node, int depth)
{
    for (int i = 0; i < depth; i++)
        putchar('\t');
    printf("Type: %s Value: %s\n", CONCRETE_NODE_TYPE_STRING[node.type], node.token == NULL ? "(null)" : node.token->str);
}

void traverse_cst(CSTNode cst, int depth)
{
    print_cst_node(cst, depth++);
    printf("Num children: %zu\n", cst.num_children);
    for (size_t i = 0; i < cst.num_children; i++)
        traverse_cst(*cst.children[i], depth);
}
