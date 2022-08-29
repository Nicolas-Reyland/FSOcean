//
// Created by Nicolas Reyland on 8/25/22.
//

#include "executable/exec_for_loop.h"
#include "eval/variables.h"
#include "executable/executable_flags.h"
#include "executable/executable.h"

int exec_for_loop(struct ExecForLoop for_loop) {
    int result = 0;
    for (size_t i = 0; i < for_loop.wordlist_len; i++) {
        assign_scope_variable(for_loop.var_name, for_loop.var_name_len, for_loop.wordlist[i]);
        result = exec_executables(for_loop.body_commands, for_loop.num_body_commands);
        if (result & EXEC_FLAG_LOOP_BREAK) {
            result -= EXEC_FLAG_LOOP_BREAK;
            break;
        }
    }
    return result;
}
