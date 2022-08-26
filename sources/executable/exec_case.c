//
// Created by Nicolas Reyland on 8/25/22.
//

#include <string.h>
#include "executable/exec_case.h"
#include "executable/exec_command.h"

int exec_case(struct ExecCase case_stat) {
    for (size_t i = 0; i < case_stat.num_patterns; i++) {
        if (strcmp(case_stat.word, case_stat.patterns[i]) == 0) {
            return exec_commands(case_stat.cases[i], case_stat.cases_lens[i]);
        }
    }
    return exec_commands(case_stat.default_case, case_stat.cases_lens[case_stat.num_patterns]);
}
