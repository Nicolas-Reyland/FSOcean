//
// Created by nicolas on 22. 8. 26.
//

#include "executable/exec_multi_executables.h"
#include "executable/executable.h"

int exec_multi_executables(struct ExecMultiExecutables multi) {
    return exec_executables(multi.executables, multi.num_executables);
}
