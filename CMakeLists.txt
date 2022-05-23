cmake_minimum_required(VERSION 3.21)
project(Ocean C)

set(CMAKE_C_STANDARD 11)

include_directories(headers)

add_executable(Ocean main.c headers/lexer/token.h sources/lexer/token.c headers/lexer/states.h headers/lexer/char_categories.h sources/lexer/char_categories.c sources/lexer/states.c headers/eval/shellstring.h sources/eval/shellstring.c headers/combinators/combinator.h sources/combinators/combinator.c sources/combinators/stack.c headers/combinators/stack.h sources/parser/ast.c headers/parser/ast.h sources/parser/cst.c headers/parser/cst.h sources/impl.c headers/impl.h sources/parser/parse_context.c headers/parser/parse_context.h sources/parser/parser.c headers/parser/parser.h)
add_executable(OceanDebug main.c headers/lexer/token.h sources/lexer/token.c headers/lexer/states.h headers/lexer/char_categories.h sources/lexer/char_categories.c sources/lexer/states.c headers/eval/shellstring.h sources/eval/shellstring.c headers/combinators/combinator.h sources/combinators/combinator.c sources/combinators/stack.c headers/combinators/stack.h sources/parser/ast.c headers/parser/ast.h sources/parser/cst.c headers/parser/cst.h sources/impl.c headers/impl.h sources/parser/parse_context.c headers/parser/parse_context.h sources/parser/parser.c headers/parser/parser.h)
add_executable(OceanTokens main.c headers/lexer/token.h sources/lexer/token.c headers/lexer/states.h headers/lexer/char_categories.h sources/lexer/char_categories.c sources/lexer/states.c headers/eval/shellstring.h sources/eval/shellstring.c headers/combinators/combinator.h sources/combinators/combinator.c sources/combinators/stack.c headers/combinators/stack.h sources/parser/ast.c headers/parser/ast.h sources/parser/cst.c headers/parser/cst.h sources/impl.c headers/impl.h sources/parser/parse_context.c headers/parser/parse_context.h sources/parser/parser.c headers/parser/parser.h)

target_compile_definitions(OceanDebug PUBLIC OCEAN_DEBUG_MACRO)
target_compile_definitions(OceanTokens PUBLIC OCEAN_DEBUG_TOKENS_MACRO)
