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
    char ** patterns;
    size_t num_patterns;
    struct Executable ** cases;
    struct Executable * default_case;
    size_t * cases_lens; // lengths of the cases
};

int exec_case(struct ExecCase);

#endif // OCEAN_EXEC_CASE_H
