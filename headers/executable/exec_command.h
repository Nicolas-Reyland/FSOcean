//
// Created by Nicolas Reyland on 8/25/22.
//

#ifndef OCEAN_EXEC_COMMAND_H
#define OCEAN_EXEC_COMMAND_H

#include <stddef.h>

struct ExecCommand {
    void * _;
};

int exec_command(struct ExecCommand);
int exec_commands(struct ExecCommand *, size_t);

#endif // OCEAN_EXEC_COMMAND_H
