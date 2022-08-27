//
// Created by Nicolas Reyland on 8/28/22.
//

#include "executable/exec_redirect.h"

int exec_redirect(struct ExecRedirect redirect) {
    // TODO: empty function
    // redirect streams
    int result = exec_executable(redirect.executable);
    // reset streams
    return result;
}
