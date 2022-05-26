//
// Created by nicolas on 5/10/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer/token.h"
#include "lexer/char_categories.h"

#define MAX_NUM_TOKENS 256
#define MAX_TOKEN_STR_LENGTH 128

static char ** split_content_into_lines(const char * content, int * num_lines);
static void tokenize_line(const char * restrict line, int line_index, Token * restrict tokens, size_t * num_tokens, int mode);
static bool can_form_operator(Token token, char c, CHAR_CATEGORY category);
static bool can_start_operator(char c, CHAR_CATEGORY category);

Token * tokenize(const char * content, size_t * num_tokens) {
    // Validate args
    if (content == NULL) {
        fprintf(stderr, "Content is NULL\n");
        exit(1);
    }
    if (num_tokens == NULL) {
        fprintf(stderr, "Num_tokens is NULL\n");
        exit(1);
    }
    // New-line escapes
    int num_lines = 0;
    char ** lines = split_content_into_lines(content, &num_lines);
    // Tokenization
    Token tokens[MAX_NUM_TOKENS];
    *num_tokens = 0;
    int mode = 0; // normal mode (as opposed to here-document mode)
    for (int line_index = 0; line_index < num_lines; line_index++)
        tokenize_line(lines[line_index], line_index, tokens, num_tokens, mode);

    // Tokens from stack to heap
    size_t mem_size = (*num_tokens) * sizeof(Token);
    Token * heap_tokens = malloc(mem_size);
    memcpy(heap_tokens, tokens, mem_size);
    return heap_tokens;
}

/*
 * Tokenize a line.
 *
 * Takes a line as input and extends the 'tokens' array. 'num_tokens' is updated accordingly.
 * The 'mode' variable determines the tokenization mode. If it is set to zero (0), the line should
 * be tokenized as a normal shell command language program line. If it is set to one (1), the
 * line should be tokenized as a here-document line.
 * If 'mode' is set to another value, the programs exists with a non-zero return code (normally 1).
 *
 * Nothing is returned since everything is updated using pointers
 */
static void tokenize_line(const char * restrict line, const int line_index, Token * restrict tokens, size_t * num_tokens, int mode)
{
    const size_t line_length = strlen(line);
    size_t char_index = 0;
    char buffer[MAX_TOKEN_STR_LENGTH];
    Token curr_token = {
            .str = buffer,
            .str_len = 0,
            .type = -1,
            .line_index = line_index,
            .char_index = -1,
    };
    enum AtomType prev_token_type = -1,
                  curr_token_type = -1;
    CHAR_CATEGORY prev_c_category = CHAR_GENERAL,
                  curr_c_category = CHAR_GENERAL;
    bool quoting = false;
    char curr_c = 0, prev_c = 0;
    while (char_index < line_length) {
        curr_c = line[char_index];
        curr_c_category = char_category(curr_c);

        // Rule 1
        // (see after while loop)

        // Rule 2
        if (prev_token_type == OPERATOR_TOKEN && !quoting && can_form_operator(curr_token, curr_c, curr_c_category)) {
            //
        }

        // Rule 3
        if (prev_token_type == OPERATOR_TOKEN && !can_form_operator(curr_token, curr_c, curr_c_category)) {
            //
        }

        // Rule 4
        if (!quoting && (curr_c == '\\' || curr_c == '"' || curr_c != '\'')) {
            //
        }

        // Rule 5
        if (!quoting && (curr_c == '$' || curr_c == '`')) {
            char next_char = char_index != line_length - 1 ? line[char_index + 1] : 0;
            char next_char_two = char_index <= line_length - 2 ? line[char_index + 2] : 0;
            if (curr_c == '$' && next_char == '{') {
                // Parameter Expansion
            } else if (curr_c == '`' || (curr_c == '$' && next_char == '(' && next_char_two != '(')) {
                // Command Substitution
            } else if (curr_c == '$' && next_char == '(' && next_char_two == '(') {
                // Arithmetic Expansion
            }
        }

        // Rule 6
        if (!quoting &&
            !can_form_operator(curr_token, curr_c, curr_c_category) &&
            can_start_operator(curr_c, curr_c_category)) {
            //
        }

        // Rule 7
        if (!quoting && curr_c_category == CHAR_BLANK) {
            //
        }

        // Rule 8
        if (curr_token_type == WORD_TOKEN) {
            //
        }

        // Rule 9
        if (curr_c == '#') {
            //
        }

        // Rule 10
        curr_token = (Token) {
            .type = WORD_TOKEN,
            .str = NULL,
            .str_len = 0,
            .char_index = char_index,
        };

        EndOfRules:
        prev_c = curr_c;
        prev_c_category = curr_c_category;
        char_index++;
    }
    // terminate current Token

}

/*
 * Can the character c be part of the token that is being formed as an operator token ?
 * Meaning: if the character c is appended at the end of the token, is it still a valid
 * operator ?
 *
 * Note: If the token is not an operator token, returns false.
 */
static bool can_form_operator(Token token, char c, CHAR_CATEGORY category)
{
    return false;
}

/*
 * Can the character c be the start of a new operator token ?
 */
static bool can_start_operator(char c, CHAR_CATEGORY category)
{
    return false;
}

/*
 * Splits the input into lines
 * Escaped new-line characters will be removed from the resulting lines, which
 * are not split by these escaped new-lines, but only by normal new-line characters.
 *
 * The new-line characters remain in the lines
 *
 * Returns the number of lines
 */
static char ** split_content_into_lines(const char * content, int * num_lines_ptr)
{
    if (content == NULL) {
        fprintf(stderr, "split_content_into_lines: Content is NULL\n");
        exit(1);
    }
    size_t content_length = strlen(content),
           content_index = 0,
           current_line_length = 0;
    int num_lines = 0;
    // at least one line
    char ** lines = malloc(sizeof(char *));
    // set line max length to the max length of the whole content
    // thus making sure we have enough space for any line, even very long ones (num of chars < max_size_t)
    char * line = malloc(content_length * sizeof(char));
    // Add line to lines
    lines[num_lines++] = line;
    while (content_index < content_length) {
        char c = content[content_index];
        if (c == '\n') {
            // end current line
            line[current_line_length] = '\n';
            line[current_line_length + 1] = 0;
            current_line_length = 0;
            // allocate memory for new line
            line = malloc(content_length * sizeof(char));
            lines = realloc(lines, (num_lines + 1) * sizeof(char*));
            // Add new line to lines
            lines[num_lines++] = line;
        }
        else if (c != '\\' || content_index + 1 == content_length || content[content_index + 1] != '\n') {
            // NOT an escaped new-line char
            line[current_line_length++] = c;
        }
        content_index++;
    }
    // end the last line
    line[current_line_length] = 0;
    *num_lines_ptr = num_lines;
    return lines;
}
