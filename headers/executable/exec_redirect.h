//
// Created by Nicolas Reyland on 8/28/22.
//

#ifndef OCEAN_EXEC_REDIRECT_H
#define OCEAN_EXEC_REDIRECT_H

#include <stddef.h>

#define REDIRECT_OUTPUT_SIMPLE_FLAG 0b0000001 // >
#define REDIRECT_OUTPUT_APPEND_FLAG 0b0000010 // >>
#define REDIRECT_INPUT_FLAG         0b0000100 // <
#define REDIRECT_DUP_INPUT_FLAG     0b0001000 // >&
#define REDIRECT_DUP_OUTPUT_FLAG    0b0010000 // <&
#define REDIRECT_CLOBBER_FLAG       0b0100000 // >|
#define REDIRECT_READ_WRITE_FLAG    0b1000000 // <>

#define REDIRECT_FILE_DESCRIPTOR_MASK 0xff000000

struct Executable;

struct ExecRedirect {
    struct Executable * executable;
    unsigned long * flags;
    char ** files;
    size_t num_redirects;
};

int exec_redirect(struct ExecRedirect);

#endif // OCEAN_EXEC_REDIRECT_H
