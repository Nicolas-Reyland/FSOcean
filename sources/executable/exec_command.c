//
// Created by Nicolas Reyland on 8/25/22.
//

#include "executable/exec_command.h"
#include "executable/executable_flags.h"
#include "executable/executable.h"

int exec_command(struct ExecCommand command)
{
    (void)command;
    return 0;
}

int exec_commands(struct ExecCommand * commands, size_t num_commands)
{
    int last_result = 0;
    for (size_t i = 0; i < num_commands && (last_result = exec_command(commands[i]) & EXEC_FLAG_EXIT_CODE_MASK) == 0; i++);
    return last_result;
}

struct Executable create_exec_command(struct ExecCommandWord * words, size_t num_words) {
    return (struct Executable) {
            .type = EXEC_COMMAND,
            .executable = (union ExecutableUnion) {
                    .command = (struct ExecCommand) {
                            .words = words,
                            .num_words = num_words,
                    }
            },
    };
}

struct ExecCommandWord create_exec_command_word(enum ExecCommandWordType type, char *str, size_t str_len) {
    return (struct ExecCommandWord) {
            .type = type,
            .str = str,
            .str_len = str_len,
    };
}

struct Executable empty_exec_command(void) {
    return (struct Executable) {
            .type = EXEC_COMMAND,
            .executable = (union ExecutableUnion) {
                .command = (struct ExecCommand) {
                    .words = NULL,
                    .num_words = 0,
                }
            },
    };
}
