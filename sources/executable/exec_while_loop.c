//
// Created by Nicolas Reyland on 8/25/22.
//

#include "executable/exec_while_loop.h"
#include "executable/exec_command.h"
#include "executable/executable_flags.h"

int exec_while_loop(struct ExecWhileLoop while_loop) {
    int result = 0;
    while ((result = exec_commands(while_loop.condition_commands, while_loop.num_condition_commands) & EXEC_FLAG_EXIT_CODE_MASK) == 0) {
        result = exec_commands(while_loop.body_commands, while_loop.num_body_commands);
        if (result & EXEC_FLAG_LOOP_BREAK)
            break;
    }
    return result;
}
