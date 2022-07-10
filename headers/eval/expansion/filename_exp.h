//
// Created by nicolas on 22. 7. 10.
//

#ifndef OCEAN_FILENAME_EXP_H
#define OCEAN_FILENAME_EXP_H

#include <sys/types.h>

ssize_t expand_filename(const char * str, size_t str_len, char *** result);

#endif //OCEAN_FILENAME_EXP_H
