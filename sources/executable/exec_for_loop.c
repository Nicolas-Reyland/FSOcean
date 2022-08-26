//
// Created by Nicolas Reyland on 8/25/22.
//

#include "executable/exec_for_loop.h"
#include "eval/variables.h"
#include "executable/exec_command.h"
#include "executable/executable_flags.h"

static char ** for_loop_elements(const struct ExecCommand *, size_t, size_t *);

int exec_for_loop(struct ExecForLoop for_loop) {
    int result = 0;
    size_t num_values;
    char ** values = for_loop_elements(for_loop.element_commands, for_loop.num_element_commands, &num_values);
    for (size_t i = 0; i < num_values; i++) {
        assign_scope_variable(for_loop.var_name, for_loop.var_name_len, values[i]);
        result = exec_commands(for_loop.body_commands, for_loop.num_body_commands);
        if (result & EXEC_FLAG_LOOP_BREAK) {
            result -= EXEC_FLAG_LOOP_BREAK;
            break;
        }
    }
    return result;
}

char ** for_loop_elements(const struct ExecCommand * element_commands, size_t num_element_commands, size_t * num_values) {
    (void)element_commands;
    (void)num_element_commands;
    (void)num_values;
    return NULL;
}
