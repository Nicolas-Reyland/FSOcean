//
// Created by Nicolas Reyland on 8/28/22.
//

#include "parser/abstractions/imperfect_abstract_io_redirect.h"
#include "parser/abstractions/abstraction.h"
#include "misc/output.h"


static void imperfect_abstract_io_file(CSTNode io_redirect, unsigned long * flags, char ** file);
static void imperfect_abstract_io_here(CSTNode io_redirect, unsigned long * flags, char ** file);

void imperfect_abstract_io_redirect(CSTNode io_redirect, unsigned long * flags, char ** file) {
    // TODO: empty function
    PARENT_NODE_COMPLIANCE(io_redirect, IO_REDIRECT_PARSER, 1)
    if (io_redirect.children[0]->type == IO_FILE_PARSER) {
        return imperfect_abstract_io_file(io_redirect, flags, file);
    }
    if (io_redirect.children[0]->type == IO_HERE_PARSER) {
        return imperfect_abstract_io_here(io_redirect, flags, file);
    }
    print_error(OCERR_EXIT, "Unexpected type for io_redirect-child: '%d'\n", io_redirect.children[0]->type);
}

static void imperfect_abstract_io_file(CSTNode io_redirect, unsigned long * flags, char ** file)
{
    // TODO: empty function
    NOT_IMPLEMENTED_ERROR(imperfect abstract io-file)
}

static void imperfect_abstract_io_here(CSTNode io_redirect, unsigned long * flags, char ** file)
{
    // TODO: empty function
    NOT_IMPLEMENTED_ERROR(imperfect abstract io-here)
}
