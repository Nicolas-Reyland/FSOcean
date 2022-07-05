//
// Created by Nicolas Reyland on 5/27/22.
//

#ifndef OCEAN_INTERACTIVE_H
#define OCEAN_INTERACTIVE_H

#include <stdnoreturn.h>

#define INTERACTIVE_TOKENS  0b00000001
#define INTERACTIVE_CST     0b00000010

noreturn void interactive_mode(long flags);

#endif // OCEAN_INTERACTIVE_H
