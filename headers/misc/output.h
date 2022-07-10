//
// Created by nicolas on 22. 7. 1.
//

#ifndef OCEAN_OUTPUT_H
#define OCEAN_OUTPUT_H

#include <stdarg.h>
#include "parser/cst.h"

#define OCERR_EXIT   0b00001
#define OCERR_STDERR 0b00010
#define OCERR_STDOUT 0b00100

void show_output_diff(const char * theory, char * practice, size_t content_len);
void traverse_cst(CSTNode cst, int depth);
void print_error(int flags, const char * format, ...);

#endif //OCEAN_OUTPUT_H
