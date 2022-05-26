//
// Created by nicolas on 5/10/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "lexer/token.h"
#include "lexer/char_categories.h"
#include "lexer/xbd_operators.h"
#include "string_utils/string_utils.h"
#include "lexer/parameter_expansion.h"
#include "lexer/command_substitution.h"
#include "lexer/arithmetic_expansion.h"

#define MAX_NUM_TOKENS 256
#define MAX_TOKEN_STR_LENGTH 128

static char ** split_content_into_lines(const char * content, int * num_lines);
static void tokenize_line(char  *line, int line_index, Token * restrict tokens, size_t * num_tokens, int mode);

static void commit_token(int line_index, int * token_char_index, Token * tokens, size_t * num_tokens,
                         char * token_str_buffer, size_t * token_str_len, AtomType curr_token_type);

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
 * The line_index is used for the tokens attributes.
 *
 * The 'mode' variable determines the tokenization mode.
 * When set to zero (0): ordinary token recognition.
 * When set to one  (1): processing of here-documents.
 * When set to anything else: exits the program with a non-zero status code (1).
 *
 */
static void tokenize_line(char *line, const int line_index, Token * restrict tokens, size_t * num_tokens, int mode)
{
    size_t line_length = strlen(line);
    char token_str_buffer[MAX_TOKEN_STR_LENGTH];
    size_t token_str_len = 0;       // Length of token_str (at current time)
    int char_index = 0,             // character that is being processed
        token_char_index = 0;       // start of token in current line
    enum AtomType curr_token_type = -1;
    CHAR_CATEGORY curr_c_category = CHAR_GENERAL;
    bool quoting = false;           // currently quoting
    char curr_c,                    // current character
         quoting_char;              // char used for quoting (only looked if 'quoting')
    while (char_index < line_length) {
        curr_c = line[char_index];
        curr_c_category = char_category(curr_c);
        StartOfRules:

        // Rule 1
        // (see after while loop)

        // Rule 2 & 3
        if (curr_token_type == OPERATOR_TOKEN) {
            /* If the previous character was used as part of an operator ... */
            // Rule 2
            if (!quoting && can_form_operator(token_str_buffer, token_str_len, curr_c, curr_c_category)) {
                /* If the previous character was used as part of an operator
                 * and the current character is not quoted
                 * and can be used with the previous characters to form an operator,
                 * it shall be used as part of that (operator) token. */
                token_str_buffer[token_str_len++] = curr_c;
                goto NextChar;
            }
            // Rule 3
            else if (!can_form_operator(token_str_buffer, token_str_len, curr_c, curr_c_category)) {
                /* If the previous character was used as part of an operator
                 * and the current character cannot be used with the previous characters to form an operator,
                 * the operator containing the previous character shall be delimited. */
                commit_token(
                        line_index,
                        &token_char_index,
                        tokens,
                        num_tokens,
                        token_str_buffer,
                        &token_str_len,
                        curr_token_type);
                // change token-types
                curr_token_type = -1;
                goto StartOfRules;
            }
        }

        // Rule 4
        if (!quoting && is_quote(curr_c)) {
            /* If the current character is <backslash>, single-quote, or double-quote and it is not quoted,
             * it shall affect quoting for subsequent characters up to the end of the quoted text.
             *
             * The rules for quoting are as described in Quoting.
             * During token recognition no substitutions shall be actually performed,
             * and the result token shall contain exactly the characters that appear in the input
             * (except for <newline> joining), unmodified, including any embedded or enclosing quotes or substitution operators,
             * between the <quotation-mark> and the end of the quoted text.
             *
             * The token shall not be delimited by the end of the quoted field. */
            quoting = true;
            quoting_char = curr_c;
            // Go to the end of the quoting
            if (quoting_char == '\\') {
                assert(char_index != line_length - 1);
                token_str_buffer[token_str_len++] = '\\';
                token_str_buffer[token_str_len++] = line[char_index + 1];
                char_index += 2;
            } else {
                size_t start_char_index = char_index;
                char_index += (int)find_corresponding_char(
                        line + (size_t)char_index,
                        line_length - (size_t)char_index,
                        0,
                        quoting_char,
                        true);
                size_t quoted_str_len = char_index - start_char_index;
                memcpy(token_str_buffer + token_str_len, &line[start_char_index], quoted_str_len);
                token_str_len += quoted_str_len;
            }
            // Don't delimit the token, just continue.
            curr_token_type = WORD_TOKEN;
            quoting = false;
            continue;
        }

        // Rule 5
        if (!quoting && (curr_c == '$' || curr_c == '`')) {
            /* If the current character is an unquoted '$' or '`',
             * the shell shall identify the start of any candidates for
             *  - parameter expansion (Parameter Expansion),
             *  - command substitution (Command Substitution), or
             *  - arithmetic expansion (Arithmetic Expansion)
             * from their introductory unquoted character sequences:
             *  - '$' or "${",
             *  - "$(" or '`',
             *  - and "$((",
             *  respectively.
             *
             * The shell shall read sufficient input to determine the end of the unit to be expanded (as explained in the cited sections).
             *
             * While processing the characters, if instances of expansions or quoting are found nested within the substitution,
             * the shell shall recursively process them in the manner specified for the construct that is found.
             * The characters found from the beginning of the substitution to its end,
             * allowing for any recursion necessary to recognize embedded constructs,
             * shall be included unmodified in the result token,
             * including any embedded or enclosing substitution operators or quotes.
             *
             * The token shall not be delimited by the end of the substitution. */
            // Next char and char after next char
            char next_char, next_char_two;
            if (char_index != line_length - 1)
                next_char = line[char_index + 1];
            else
                next_char = 0;
            if (char_index <= line_length - 2)
                 next_char_two = line[char_index + 2];
            else
                next_char_two = 0;
            // Determine the correct candidate
            size_t start_char_index = char_index;
            if (curr_c == '$' && next_char != '(') {
                // Parameter Expansion: '$' | "${"
                if (next_char == '{') {
                    char_index += (int)parameter_expansion_end(
                            line + char_index,
                            line_length - char_index);
                } else {
                    // go to the next special char (including blanks)
                    char_index += (int)parameter_expansion_no_brackets_end(
                            line + char_index,
                            line_length - char_index);
                }
            } else if (curr_c == '`' || (next_char == '(' && next_char_two != '(')) {
                // Command Substitution: '`' | "$("
                char_index += (int)command_substitution_end(
                        line + char_index,
                        line_length - char_index,
                        curr_c);
            } else if (next_char == '(' && next_char_two == '(') {
                // Arithmetic Expansion: "$(("
                char_index += (int)arithmetic_expansion_end(
                        line + char_index,
                        line_length - char_index);
            }
            // Don't delimit the token, just continue.
            line_length = strlen(line);
            size_t offset = char_index - start_char_index;
            memcpy(token_str_buffer + token_str_len, line + start_char_index, offset);
            token_str_len += offset;
            curr_token_type = WORD_TOKEN;
            continue;
        }

        // Rule 6
        if (!quoting &&
            is_operator_start(curr_c, curr_c_category)) {
            /* If the current character is not quoted and can be used as the first character of a new operator,
             * the current token (if any) shall be delimited.
             *
             * The current character shall be used as the beginning of the next (operator) token. */
            if (token_str_len != 0)
                commit_token(
                        line_index,
                        &token_char_index,
                        tokens,
                        num_tokens,
                        token_str_buffer,
                        &token_str_len,
                        curr_token_type);
            // update token-types
            curr_token_type = OPERATOR_TOKEN;
            goto StartOfRules;
        }

        // Rule 7
        if (!quoting && is_blank_char(curr_c)) {
            /* If the current character is an unquoted <blank>,
             * any token containing the previous character is delimited
             * and the current character shall be discarded. */
            if (token_str_len != 0)
                commit_token(
                        line_index,
                        &token_char_index,
                        tokens,
                        num_tokens,
                        token_str_buffer,
                        &token_str_len,
                        curr_token_type);
            token_char_index++;
            curr_token_type = -1;
            goto NextChar;
        }

        // Rule 8
        if (curr_token_type == WORD_TOKEN) {
            /* If the previous character was part of a word, the current character shall be appended to that word. */
            token_str_buffer[token_str_len++] = curr_c;
            goto NextChar;
        }

        // Rule 9
        if (curr_c == '#') {
            /* If the current character is a '#',
             * it and all subsequent characters up to, but excluding, the next <newline> shall be discarded as a comment.
             *
             * The <newline> that ends the line is not considered part of the comment. */
            char_index = (int)line_length - 1;
            if (line[char_index] == '\n')
                continue;
            break;
        }

        // Rule 10
        /* The current character is used as the start of a new word. */
        token_str_buffer[0] = curr_c;
        token_str_len = 1;
        curr_token_type = WORD_TOKEN;
        token_char_index = char_index;

        // End of rules
        NextChar:
        char_index++;
    }
    // terminate current Token
    if (token_str_len != 0) {
        /* If the end of input is recognized, the current token (if any) shall be delimited. */
        commit_token(
                line_index,
                &token_char_index,
                tokens,
                num_tokens,
                token_str_buffer,
                &token_str_len,
                curr_token_type);
    }
}

static void commit_token(int line_index, int * token_char_index, Token * tokens, size_t * num_tokens,
                         char * token_str_buffer, size_t * token_str_len, AtomType curr_token_type)
{
    size_t num_bytes = (*token_str_len + 1);
    Token token = (Token) {
            .str = malloc(num_bytes),
            .str_len = *token_str_len,
            .char_index = *token_char_index,
            .line_index = line_index,
            .type = curr_token_type,
    };
    token_str_buffer[*token_str_len] = 0;
    memcpy(token.str, token_str_buffer, num_bytes);
    tokens[(*num_tokens)++] = token;
    // reset some vars
    *token_char_index += (int)*token_str_len;
    *token_str_len = 0;
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
    char * line = malloc(content_length);
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
            line = malloc(content_length);
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

void print_tokens(Token * tokens, size_t num_tokens)
{
    for (size_t i = 0; i < num_tokens; i++) {
        if (tokens[i].type == OPERATOR_TOKEN && tokens[i].str_len == 1 && tokens[i].str[0] == '\n')
            printf(".\n");
        else if (tokens[i].type == NEWLINE_TOKEN && tokens[i].str_len == 1 && tokens[i].str[0] == '\n')
            printf("~\n");
        else
            printf("T: (%s) '%s'\n", ATOM_TYPE_STRING(tokens[i].type), tokens[i].str);
    }
}
