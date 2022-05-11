cmake_minimum_required(VERSION 3.21)
project(Ocean C)

set(CMAKE_C_STANDARD 11)

include_directories(headers)
add_executable(Ocean main.c headers/lexer/tokens.h sources/lexer/tokens.c headers/lexer/states.h headers/lexer/char_categories.h sources/lexer/char_categories.c sources/lexer/states.c headers/eval/shellstring.h sources/eval/shellstring.c)
