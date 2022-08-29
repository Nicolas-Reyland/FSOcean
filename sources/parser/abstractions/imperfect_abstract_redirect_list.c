//
// Created by Nicolas Reyland on 8/29/22.
//

#include "parser/abstractions/imperfect_abstract_redirect_list.h"
#include "parser/abstractions/abstraction.h"
#include "misc/safemem.h"
#include "parser/abstractions/imperfect_abstract_io_redirect.h"

size_t imperfect_abstract_redirect_list(CSTNode redirect_list, unsigned long ** flags, char *** files) {
    assert(*flags == NULL && *files == NULL);
    NODE_COMPLIANCE(redirect_list, REDIRECT_LIST_PARSER, 2, IO_REDIRECT_PARSER, REPETITION_PARSER)
    // Node children
    CSTNode first_io_redirect = *redirect_list.children[0];
    CSTNode repetition = *redirect_list.children[1];
    // First redirect
    size_t num_redirects = 1 + repetition.num_children;
    *flags = reg_calloc(num_redirects, sizeof(unsigned long));
    *files = reg_calloc(num_redirects, sizeof(char *));
    imperfect_abstract_io_redirect(first_io_redirect, *flags, *files);
    // Rest of redirects
    for (size_t i = 0; i < repetition.num_children; i++) {
        CSTNode io_redirect = *repetition.children[i];
        imperfect_abstract_io_redirect(io_redirect, (*flags) + i + 1, (*files) + i + 1);
    }
    return num_redirects;
}
