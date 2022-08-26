//
// Created by Nicolas Reyland on 8/25/22.
//

#include "executable/exec_until_loop.h"
#include "executable/exec_command.h"
#include "executable/executable_flags.h"

int exec_until_loop(struct ExecUntilLoop until_loop) {
    int result = 0;
    while (!EXEC_CMD_SUCCESS(exec_commands(until_loop.condition_commands, until_loop.num_condition_commands))) {
        result = exec_commands(until_loop.body_commands, until_loop.num_body_commands);
    }
    return result;
}
