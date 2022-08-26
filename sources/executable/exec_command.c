//
// Created by Nicolas Reyland on 8/25/22.
//

#include "executable/exec_command.h"
#include "executable/executable_flags.h"

int exec_command(struct ExecCommand command)
{
    (void)command;
    return 0;
}

int exec_commands(struct ExecCommand * commands, size_t num_commands)
{
    int last_result = 0;
    for (size_t i = 0; i < num_commands && (last_result = exec_command(commands[i]) & EXEC_FLAG_EXIT_CODE_MASK) == 0; i++);
    return last_result;
}
