//
// Created by Nicolas Reyland on 8/25/22.
//

#include "executable/exec_if.h"
#include "executable/executable_flags.h"

int exec_if(struct ExecIf if_stat)
{
    if (EXEC_CMD_SUCCESS(exec_executables(if_stat.condition_commands, if_stat.num_condition_commands)))
        return exec_executables(if_stat.body_commands, if_stat.num_body_commands);
    else
        return exec_executables(if_stat.else_commands, if_stat.num_else_commands);
}
