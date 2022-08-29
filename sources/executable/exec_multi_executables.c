//
// Created by nicolas on 22. 8. 26.
//

#include <assert.h>
#include "executable/exec_multi_executables.h"
#include "executable/executable.h"
#include "misc/output.h"

static int exec_multi_executables_and(struct ExecMultiExecutables);
static int exec_multi_executables_or(struct ExecMultiExecutables);
static int exec_multi_executables_pipe(struct ExecMultiExecutables);

int exec_multi_executables(struct ExecMultiExecutables multi) {
    if (multi.execution_flags & EXE_SEQUENTIAL)
        return exec_executables(multi.executables, multi.num_executables);
    if (multi.execution_flags & EXE_AND_FLAG)
        return exec_multi_executables_and(multi);
    if (multi.execution_flags & EXE_OR_FLAG)
        return exec_multi_executables_or(multi);
    if (multi.execution_flags & EXE_PIPE_FLAG)
        return exec_multi_executables_pipe(multi);
    print_error_exit("Invalid flags for multi-execution: '%d'\n", multi.execution_flags);
}

static int exec_multi_executables_and(struct ExecMultiExecutables multi)
{
    assert(multi.num_executables >= 2);
    for (size_t i = 0; i < multi.num_executables; i++) {
        int result = exec_executable(multi.executables[i]);
        if (result != 0)
            return result;
    }
    return 0;
}

static int exec_multi_executables_or(struct ExecMultiExecutables multi)
{
    assert(multi.num_executables >= 2);
    int result = 0;
    for (size_t i = 0; i < multi.num_executables; i++) {
        result = exec_executable(multi.executables[i]);
        if (result == 0)
            return result;
    }
    return result;
}

static int exec_multi_executables_pipe(struct ExecMultiExecutables multi)
{
    assert(multi.num_executables >= 2);
    assert( 0 ); // NOT IMPLEMENTED
}
