//
// Created by Nicolas Reyland on 8/25/22.
//

#include "executable/exec_while_loop.h"
#include "executable/executable_flags.h"
#include "executable/executable.h"

int exec_while_loop(struct ExecWhileLoop while_loop) {
    int result;
    while ((result = exec_executables(while_loop.condition_executables, while_loop.num_condition_executables) & EXEC_FLAG_EXIT_CODE_MASK) == 0) {
        result = exec_executables(while_loop.body_executables, while_loop.num_body_executables);
        if (result & EXEC_FLAG_LOOP_BREAK) {
            result -= EXEC_FLAG_LOOP_BREAK;
            break;
        }
    }
    return result;
}
