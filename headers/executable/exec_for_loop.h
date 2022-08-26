//
// Created by Nicolas Reyland on 8/25/22.
//

#ifndef OCEAN_EXEC_FOR_LOOP_H
#define OCEAN_EXEC_FOR_LOOP_H

#include <stddef.h>
#include "executable.h"

struct ExecForLoop {
    char * var_name;
    size_t var_name_len;
    Executable * element_commands;
    size_t num_element_commands;
    Executable * body_commands;
    size_t num_body_commands;
};

int exec_for_loop(struct ExecForLoop);

#endif // OCEAN_EXEC_FOR_LOOP_H
