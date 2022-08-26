//
// Created by Nicolas Reyland on 8/25/22.
//

#ifndef OCEAN_EXECUTABLE_H
#define OCEAN_EXECUTABLE_H

#include <stddef.h>
#include <stdbool.h>
#include "executable_union.h"
#include "executable_type.h"

typedef struct Executable Executable;

struct Executable {
    union ExecutableUnion executable;
    enum ExecutableType type;
    bool redirect;
};

#endif // OCEAN_EXECUTABLE_H
