//
// Created by Nicolas Reyland on 8/25/22.
//

#include "executable/exec_case.h"
#include "executable/executable.h"
#include "misc/output/output.h"

int exec_case(struct ExecCase case_stat) {
    NOT_IMPLEMENTED_ERROR(case clause execution)
    /*
    for (size_t i = 0; i < case_stat.num_patterns; i++)
        if (strcmp(case_stat.word, case_stat.patterns[i]) == 0)
            return exec_executables(case_stat.cases[i], case_stat.cases_lens[i]);
    return exec_executables(case_stat.default_case, case_stat.cases_lens[case_stat.num_patterns]);
     */
    return 0;
}
