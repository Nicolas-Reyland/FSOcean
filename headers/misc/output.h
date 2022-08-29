//
// Created by nicolas on 22. 7. 1.
//

#ifndef OCEAN_OUTPUT_H
#define OCEAN_OUTPUT_H

#include <stdarg.h>
#include <stdnoreturn.h>
#include "parser/cst.h"
#include "executable/executable.h"

#define OCERR_EXIT   0b00001
#define OCERR_STDERR 0b00010
#define OCERR_STDOUT 0b00100

#define NOT_IMPLEMENTED_ERROR(functionality) print_error(OCERR_EXIT, "/!\\ '" #functionality "' functionality is not implemented\n");

void show_output_diff(const char * theory, char * practice, size_t content_len);
void traverse_cst(CSTNode cst, int depth);
void traverse_executable(Executable executable, int depth);
void print_error(int flags, const char * format, ...);
noreturn void print_error_exit(const char * format, ...);

#endif //OCEAN_OUTPUT_H
