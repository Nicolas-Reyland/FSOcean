//
// Created by Nicolas Reyland on 8/25/22.
//

#include "executable/executable.h"
#include "misc/output.h"

int exec_executable(Executable executable)
{
    switch (executable.type) {
        case EXEC_COMMAND:
            return exec_command((struct ExecCommand)executable.executable);
        case EXEC_FOR_LOOP:
            return exec_for_loop((struct ExecForLoop)executable.executable);
        case EXEC_WHILE_LOOP:
            return exec_while_loop((struct ExecWhileLoop)executable.executable);
        case EXEC_UNTIL_LOOP:
            return exec_until_loop((struct ExecUntilLoop)executable.executable);
        case EXEC_CASE:
            return exec_case((struct ExecCase)executable.executable);
        case EXEC_IF:
            return exec_if((struct ExecIf)executable.executable);
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
