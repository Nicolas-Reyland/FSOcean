//
// Created by Nicolas Reyland on 8/28/22.
//

#include "executable/exec_redirect.h"
#include "executable/executable.h"

int exec_redirect(struct ExecRedirect redirect) {
    // TODO: save streams states & redirect streams
    int result = exec_executable(*redirect.executable);
    // TODO: reset streams states
    return result;
}
