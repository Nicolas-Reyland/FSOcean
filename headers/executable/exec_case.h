//
// Created by Nicolas Reyland on 8/25/22.
//

#ifndef OCEAN_EXEC_CASE_H
#define OCEAN_EXEC_CASE_H

#include <stddef.h>

struct Executable;

struct ExecCase {
    char * word; // the word to match against the patterns (already evaluated)
    size_t word_len;
    size_t num_patterns;
    struct Executable * cases;
};

int exec_case(struct ExecCase);

#endif // OCEAN_EXEC_CASE_H
