//
// Created by nicolas on 22. 8. 26.
//

#ifndef OCEAN_EXEC_MULTI_EXECUTABLES_H
#define OCEAN_EXEC_MULTI_EXECUTABLES_H

#include <stddef.h>

struct Executable;

struct ExecMultiExecutables {
    struct Executable * executables;
    size_t num_executables;
};

int exec_multi_executables(struct ExecMultiExecutables);

#endif //OCEAN_EXEC_MULTI_EXECUTABLES_H
