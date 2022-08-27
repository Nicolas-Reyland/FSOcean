//
// Created by Nicolas Reyland on 8/28/22.
//

#include <string.h>
#include <stdlib.h>
#include "parser/abstractions/imperfect_abstract_io_redirect.h"
#include "parser/abstractions/abstraction.h"
#include "misc/output.h"


static void imperfect_abstract_io_file(CSTNode io_file, unsigned long * flags, char ** file);
static void imperfect_abstract_io_here(CSTNode io_here, unsigned long * flags, char ** file);

void imperfect_abstract_io_redirect(CSTNode io_redirect, unsigned long * flags, char ** file) {
    NODE_COMPLIANCE(io_redirect, IO_REDIRECT_PARSER, 2, OPTIONAL_PARSER, CHOICE_PARSER)
    CSTNode io_number = *io_redirect.children[0],
            io_object = *io_redirect.children[1]; // object is 'file' or 'here'
    assert(io_object.num_children == 1);
    unsigned long io_number_mask = 0;
    if (has_children(io_number)) {
        unsigned long io_number_fd = strtoul(io_number.children[0]->token->str, NULL, 0);
        // assert(io_number_fd <= 0xff); // no overflow error
        io_number_mask = (io_number_fd & 0xff) << (3 * 8); // shrink to one byte length and shift left 3 bytes
    }
    if (io_redirect.children[0]->type == IO_FILE_PARSER) {
        imperfect_abstract_io_file(*io_object.children[0], flags, file);
        *flags |= io_number_mask;
        return;
    }
    if (io_redirect.children[0]->type == IO_HERE_PARSER) {
        imperfect_abstract_io_here(*io_object.children[0], flags, file);
        *flags |= io_number_mask;
        return;
    }
    print_error(OCERR_EXIT, "Unexpected type for io_redirect-child: '%d'\n", io_redirect.children[0]->type);
}

static void imperfect_abstract_io_file(CSTNode io_file, unsigned long * flags, char ** file)
{
    NODE_COMPLIANCE(io_file, IO_FILE_PARSER, 2, CHOICE_PARSER, FILENAME_PARSER)
    CSTNode gen_string = *io_file.children[0];
    // Test all the possible redirects
    if (strcmp(gen_string.token->str, ">") == 0)
        *flags |= REDIRECT_OUTPUT_FLAG;
    else if (strcmp(gen_string.token->str, ">>") == 0)
        *flags |= REDIRECT_OUTPUT_APPEND_FLAG;
    else if (strcmp(gen_string.token->str, "<") == 0)
        *flags |= REDIRECT_INPUT_FLAG;
    else if (strcmp(gen_string.token->str, ">&") == 0)
        *flags |= REDIRECT_DUP_OUTPUT_FLAG;
    else if (strcmp(gen_string.token->str, "<&") == 0)
        *flags |= REDIRECT_DUP_INPUT_FLAG;
    else if (strcmp(gen_string.token->str, ">|") == 0)
        *flags |= REDIRECT_CLOBBER_FLAG;
    else if (strcmp(gen_string.token->str, "<>") == 0)
        *flags |= REDIRECT_READ_WRITE_FLAG;
    else
        print_error(OCERR_EXIT, "Unexpected redirect-symbol token string '%s'\n", gen_string.token->str);
    // Set the file name
    *file = io_file.children[1]->token->str;
}

static void imperfect_abstract_io_here(CSTNode io_here, unsigned long * flags, char ** file)
{
    // TODO: empty function
    NOT_IMPLEMENTED_ERROR(imperfect abstract io-here)
}
