//
// Created by Nicolas Reyland on 8/25/22.
//

#ifndef OCEAN_EXEC_WHILE_LOOP_H
#define OCEAN_EXEC_WHILE_LOOP_H

#include <stddef.h>

struct Executable;

struct ExecWhileLoop {
    struct Executable * condition_commands;
    size_t num_condition_commands;
    struct Executable * body_commands;
    size_t num_body_commands;
};

int exec_while_loop(struct ExecWhileLoop);

#endif // OCEAN_EXEC_WHILE_LOOP_H
