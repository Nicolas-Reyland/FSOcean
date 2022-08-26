//
// Created by Nicolas Reyland on 8/25/22.
//

#ifndef OCEAN_EXEC_IF_H
#define OCEAN_EXEC_IF_H

#include <stddef.h>
#include "executable.h"

struct ExecIf {
    Executable * condition_commands;
    size_t num_condition_commands;
    Executable * body_commands;
    size_t num_body_commands;
    Executable * else_commands;
    size_t num_else_commands;
    /*
     * For elif, you can "hide" another if-statement inside the else part
     *
     * like so :
     * A ->
     *      "if X then
     *          0
     *       elif Y then
     *          1
     *       else
     *          2
     *       fi"
     *
     * B ->
     *      "if X then
     *          0
     *       else
     *          if Y then
     *              1
     *          else
     *              2
     *          fi
     *       fi"
     */
};

int exec_if(struct ExecIf);

#endif // OCEAN_EXEC_IF_H
