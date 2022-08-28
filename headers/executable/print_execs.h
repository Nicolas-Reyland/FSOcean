//
// Created by Nicolas Reyland on 8/29/22.
//

#ifndef OCEAN_PRINT_EXECS_H
#define OCEAN_PRINT_EXECS_H

#include "executable.h"

void print_exec_command(struct ExecCommand command, int depth);
void print_exec_for_loop(struct ExecForLoop for_loop, int depth);
void print_exec_while_loop(struct ExecWhileLoop while_loop, int depth);
void print_exec_until_loop(struct ExecUntilLoop until_loop, int depth);
void print_exec_case(struct ExecCase case_stat, int depth);
void print_exec_if(struct ExecIf if_stat, int depth);
void print_exec_multi(struct ExecMultiExecutables multi, int depth);
void print_exec_redirect(struct ExecRedirect redirect, int depth);

#endif // OCEAN_PRINT_EXECS_H
