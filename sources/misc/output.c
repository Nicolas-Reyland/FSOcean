//
// Created by nicolas on 22. 7. 1.
//

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "misc/output.h"
#include "executable/print_execs.h"

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

void traverse_executable(Executable executable, int depth)
{
    switch (executable.type) {
        case EXEC_COMMAND:
            print_exec_command(executable.executable.command, depth);
            break;
        case EXEC_FOR_LOOP:
            print_exec_for_loop(executable.executable.for_loop, depth);
            break;
        case EXEC_WHILE_LOOP:
            print_exec_while_loop(executable.executable.while_loop, depth);
            break;
        case EXEC_UNTIL_LOOP:
            print_exec_until_loop(executable.executable.until_loop, depth);
            break;
        case EXEC_CASE:
            print_exec_case(executable.executable.case_clause, depth);
            break;
        case EXEC_IF:
            print_exec_if(executable.executable.if_clause, depth);
            break;
        case EXEC_MULTI:
            print_exec_multi(executable.executable.multi, depth);
            break;
        case EXEC_REDIRECT:
            print_exec_redirect(executable.executable.redirect, depth);
            break;
        default:
            print_error(OCERR_EXIT, "Unexpected type for executable: '%d'\n", executable.type);
    }
}
#pragma clang diagnostic pop

void print_error(int flags, const char * format, ...)
{
    va_list args;
    va_start(args, format);

    if (flags & OCERR_STDOUT) {
        fprintf(stdout, "error: ");
        vfprintf(stdout, format, args);
    } else
        goto PrintErrorStderr;
    if (flags & OCERR_STDERR) {
        PrintErrorStderr:
        fprintf(stderr, "error: ");
        vfprintf(stderr, format, args);
    }
    if (flags & OCERR_EXIT) {
        va_end(args);
        exit(EXIT_FAILURE);
    }

    va_end(args);
}

noreturn void print_error_exit(const char * format, ...)
{
    va_list args;
    va_start(args, format);

    fprintf(stderr, "error: ");
    vfprintf(stderr, format, args);

    va_end(args);
    exit(EXIT_FAILURE);
}
