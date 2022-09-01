//
// Created by Nicolas Reyland on 8/25/22.
//

#ifndef OCEAN_EXEC_WHILE_LOOP_H
#define OCEAN_EXEC_WHILE_LOOP_H

#include <stddef.h>

struct Executable;

struct ExecWhileLoop {
    struct Executable * condition_executables;
    size_t num_condition_executables;
    struct Executable * body_executables;
    size_t num_body_executables;
};

int exec_while_loop(struct ExecWhileLoop);

#endif // OCEAN_EXEC_WHILE_LOOP_H
