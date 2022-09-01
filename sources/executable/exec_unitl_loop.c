//
// Created by Nicolas Reyland on 8/25/22.
//

#include "executable/exec_until_loop.h"
#include "executable/executable_flags.h"
#include "executable/executable.h"

int exec_until_loop(struct ExecUntilLoop until_loop) {
    int result = 0;
    while (!EXEC_CMD_SUCCESS(exec_executables(until_loop.condition_executables, until_loop.num_condition_executables))) {
        result = exec_executables(until_loop.body_executables, until_loop.num_body_executables);
    }
    return result;
}
