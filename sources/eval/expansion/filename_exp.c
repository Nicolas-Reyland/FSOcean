//
// Created by nicolas on 22. 7. 10.
//

#include <string.h>
#include "eval/expansion/filename_exp.h"
#include "misc/safemem.h"

ssize_t expand_filename(const char * str, size_t str_len, char *** result)
{
    *result = reg_malloc(sizeof(char *));
    **result = reg_malloc(str_len + 1);
    memcpy(**result, str, str_len + 1);
    return 1;
}
