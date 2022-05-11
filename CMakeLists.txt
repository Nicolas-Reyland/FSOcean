cmake_minimum_required(VERSION 3.21)
project(Ocean C)

set(CMAKE_C_STANDARD 11)

include_directories(headers)
add_executable(Ocean main.c headers/lexer/token.h sources/lexer/token.c headers/lexer/states.h headers/lexer/char_categories.h sources/lexer/char_categories.c sources/lexer/states.c headers/eval/shellstring.h sources/eval/shellstring.c headers/parser/parser.h sources/parser/parser.c sources/parser/token_stack.c headers/parser/token_stack.h sources/parser/ast.c headers/parser/ast.h)
