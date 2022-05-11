cmake_minimum_required(VERSION 3.21)
project(FSOcean C)

set(CMAKE_C_STANDARD 11)

include_directories(headers)
add_executable(FSOcean main.c headers/tokens.h sources/tokens.c headers/states.h headers/char_categories.h sources/char_categories.c sources/states.c headers/shellstring.h sources/shellstring.c)
