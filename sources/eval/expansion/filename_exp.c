//
// Created by nicolas on 22. 7. 10.
//

#include <stdlib.h>
#include <string.h>
#include "eval/expansion/filename_exp.h"

ssize_t expand_filename(const char * str, size_t str_len, char *** result)
{
    *result = malloc(sizeof(char *));
    **result = malloc(str_len + 1);
    memcpy(**result, str, str_len + 1);
    return 1;
}
