//
// Created by nicolas on 22. 8. 26.
//

#ifndef OCEAN_EXEC_MULTI_EXECUTABLES_H
#define OCEAN_EXEC_MULTI_EXECUTABLES_H

#include <stddef.h>

#define EXE_AND_FLAG    0b00001
#define EXE_OR_FLAG     0b00010
#define EXE_PIPE_FLAG   0b00100

struct Executable;

struct ExecMultiExecutables {
    unsigned int execution_flags;
    struct Executable * executables;
    size_t num_executables;
};

int exec_multi_executables(struct ExecMultiExecutables);

#endif //OCEAN_EXEC_MULTI_EXECUTABLES_H
