//
// Created by nicolas on 22. 8. 26.
//

#ifndef OCEAN_EXEC_MULTI_EXECUTABLES_H
#define OCEAN_EXEC_MULTI_EXECUTABLES_H

#include <stddef.h>

#define EXE_SEQUENTIAL  0b00000000001
#define EXE_AND_FLAG    0b00000000010
#define EXE_OR_FLAG     0b00000000100
#define EXE_PIPE_FLAG   0b00000001000
#define EXE_CASE_ITEM   0b00000010000
#define EXE_SUBSHELL    0b00000100000
#define EXE_BRACE_GROUP 0b00001000000

struct Executable;

struct ExecMultiExecutables {
    unsigned int execution_flags;
    struct Executable * executables;
    size_t num_executables;
};

int exec_multi_executables(struct ExecMultiExecutables);
struct Executable create_exec_multi_executables(unsigned int execution_flags, struct Executable * executables, size_t num_executables);

#endif //OCEAN_EXEC_MULTI_EXECUTABLES_H
