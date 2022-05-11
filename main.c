#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <fcntl.h>
#include "lexer/token.h"

#define FILE_CONTENT_BUFFER_SIZE 256

int main(int argc, char** argv) {
    // read file from cmd arg
    if (argc != 2) {
        fprintf(stderr, "need exactly one command-line argument: filename\n");
        exit(1);
    }
    char* filename = argv[1];
    int input_file = open(filename, O_RDONLY, 0444);
    char file_content[FILE_CONTENT_BUFFER_SIZE] = {};
    ssize_t cursor = read(input_file, file_content, FILE_CONTENT_BUFFER_SIZE);
    if (cursor == FILE_CONTENT_BUFFER_SIZE) {
        fprintf(stderr, "File is too big\n");
        close(input_file);
        exit(1);
    }
    file_content[cursor] = 0x0;
    // tokenize content
    size_t num_tokens = 0;
    Token * tokens = tokenize(file_content, &num_tokens);

    for (size_t i = 0; i < num_tokens; i++) {
        if (!tokens[i].eof_or_empty)
            printf("Token[%zu] (%s): %s\n", i, STATE_STRING(tokens[i].state), tokens[i].str);
    }

    return 0;
}
