//
// Created by Nicolas Reyland on 8/25/22.
//

#ifndef OCEAN_EXECUTABLE_UNION_H
#define OCEAN_EXECUTABLE_UNION_H

#include "exec_command.h"
#include "exec_case.h"
#include "exec_if.h"
#include "exec_for_loop.h"
#include "exec_while_loop.h"
#include "exec_until_loop.h"
#include "exec_multi_executables.h"
#include "exec_redirect.h"

union ExecutableUnion {
    struct ExecCommand command;
    struct ExecCase case_stat;
    struct ExecIf if_stat;
    struct ExecForLoop for_loop;
    struct ExecWhileLoop while_loop;
    struct ExecUntilLoop until_loop;
    struct ExecMultiExecutables multi;
    struct ExecRedirect redirect;
};

#endif // OCEAN_EXECUTABLE_UNION_H
