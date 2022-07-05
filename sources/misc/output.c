//
// Created by nicolas on 22. 7. 1.
//

#include <stdio.h>
#include <assert.h>
#include "misc/output.h"

void show_output_diff(const char * theory, char * practice, size_t content_len)
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

static void print_cst_node(CSTNode node, int depth)
{
    for (int i = 0; i < depth; i++)
        putchar('\t');
    printf("%d cst_%s : %s\n",
           depth,
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
#pragma clang diagnostic pop

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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
void traverse_ast(ASTNode ast, int depth)
{
    print_ast_node(ast, depth++);
    for (size_t i = 0; i < ast.num_children; i++)
        traverse_ast(ast.children[i], depth);
}
#pragma clang diagnostic pop
*/