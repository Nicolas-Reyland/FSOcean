//
// Created by Nicolas Reyland on 8/28/22.
//

#ifndef OCEAN_IMPERFECT_ABSTRACT_IO_REDIRECT_H
#define OCEAN_IMPERFECT_ABSTRACT_IO_REDIRECT_H

#include "parser/cst.h"
#include "executable/exec_redirect.h"

void imperfect_abstract_io_redirect(CSTNode io_redirect, unsigned long * flags, char ** file);

#endif // OCEAN_IMPERFECT_ABSTRACT_IO_REDIRECT_H
