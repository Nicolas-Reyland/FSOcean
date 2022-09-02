//
// Created by Nicolas Reyland on 8/25/22.
//

#ifndef OCEAN_EXEC_FOR_LOOP_H
#define OCEAN_EXEC_FOR_LOOP_H

#include <stddef.h>

struct Executable;

struct ExecForLoop {
    char * var_name;
    size_t var_name_len;
    char ** wordlist;
    size_t wordlist_len;
    struct Executable * body_executables;
    size_t num_body_executables;
};

int exec_for_loop(struct ExecForLoop);

#endif // OCEAN_EXEC_FOR_LOOP_H
