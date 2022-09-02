//
// Created by Nicolas Reyland on 8/25/22.
//

#ifndef OCEAN_EXEC_IF_H
#define OCEAN_EXEC_IF_H

#include <stddef.h>

struct Executable;

struct ExecIf {
    struct Executable * condition_executables;
    size_t num_condition_executables;
    struct Executable * body_executables;
    size_t num_body_executables;
    struct Executable * else_executables;
    size_t num_else_executables;
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
