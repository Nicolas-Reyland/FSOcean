//
// Created by Nicolas Reyland on 8/25/22.
//

#include "executable/executable.h"
#include "misc/output/output.h"

int exec_executable(Executable executable)
{
    switch (executable.type) {
        case EXEC_COMMAND:
            return exec_command(executable.executable.command);
        case EXEC_FOR_LOOP:
            return exec_for_loop(executable.executable.for_loop);
        case EXEC_WHILE_LOOP:
            return exec_while_loop(executable.executable.while_loop);
        case EXEC_UNTIL_LOOP:
            return exec_until_loop(executable.executable.until_loop);
        case EXEC_CASE:
            return exec_case(executable.executable.case_clause);
        case EXEC_IF:
            return exec_if(executable.executable.if_clause);
        case EXEC_MULTI:
            return exec_multi_executables(executable.executable.multi);
        case EXEC_REDIRECT:
            return exec_redirect(executable.executable.redirect);
        default:
            print_error(OCERR_STDERR | OCERR_EXIT, "Unknown executable type %d\n", executable.type);
            return 0xff;
    }
}

int exec_executables(Executable * executables, size_t num_executables)
{
    int result = 0;
    for (size_t i = 0; i < num_executables; i++) {
        result = exec_executable(executables[i]);
    }
    return result;
}
