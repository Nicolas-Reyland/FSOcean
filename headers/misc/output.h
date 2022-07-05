//
// Created by nicolas on 22. 7. 1.
//

#ifndef OCEAN_OUTPUT_H
#define OCEAN_OUTPUT_H

#include "parser/cst.h"


void show_output_diff(const char * theory, char * practice, size_t content_len);
void traverse_cst(CSTNode cst, int depth);

#endif //OCEAN_OUTPUT_H
