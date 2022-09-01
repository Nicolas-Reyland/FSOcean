//
// Created by Nicolas Reyland on 8/25/22.
//

#include "executable/exec_if.h"
#include "executable/executable_flags.h"
#include "executable/executable.h"

int exec_if(struct ExecIf if_stat)
{
    if (EXEC_CMD_SUCCESS(exec_executables(if_stat.condition_executables, if_stat.num_condition_executables)))
        return exec_executables(if_stat.body_executables, if_stat.num_body_executables);
    else
        return exec_executables(if_stat.else_executables, if_stat.num_else_executables);
}
