//
// Created by Nicolas Reyland on 8/25/22.
//

#ifndef OCEAN_EXEC_COMMAND_H
#define OCEAN_EXEC_COMMAND_H

#include <stddef.h>

struct ExecCommandWord;
enum ExecCommandWordType;

struct ExecCommand {
    struct ExecCommandWord * words;
    size_t num_words;
};

enum ExecCommandWordType {
    ECW_WORD, // to be evaluated, etc
    ECW_LOGIC_OPERATOR, // && or ||
    ECW_AS_IS, // not to be evaluated
    ECW_ASSIGNMENT, // assignment
    ECW_PATTERN, // pattern (case clause)
};

struct ExecCommandWord {
    enum ExecCommandWordType type;
    char * str;
    size_t str_len;
};

int exec_command(struct ExecCommand);
int exec_commands(struct ExecCommand *, size_t);

struct Executable create_exec_command(struct ExecCommandWord * words, size_t num_words);
struct ExecCommandWord create_exec_command_word(enum ExecCommandWordType type, char * str, size_t str_len);
struct Executable empty_exec_command(void);

#endif // OCEAN_EXEC_COMMAND_H
