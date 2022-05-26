cmake_minimum_required(VERSION 3.21)
project(Ocean C)

set(CMAKE_C_STANDARD 11)

include(CTest)

include_directories(headers)

add_executable(Ocean main.c sources/impl.c headers/impl.h headers/lexer/token.h sources/lexer/token.c headers/lexer/char_categories.h sources/lexer/char_categories.c headers/eval/string_eval.h sources/eval/string_eval.c headers/combinators/combinator.h sources/combinators/combinator.c sources/combinators/stack.c headers/combinators/stack.h sources/parser/ast.c headers/parser/ast.h sources/parser/cst.c headers/parser/cst.h sources/parser/parse_context.c headers/parser/parse_context.h sources/parser/parser.c headers/parser/parser.h headers/atom_type.h sources/lexer/xbd_operators.c headers/lexer/xbd_operators.h sources/string_utils/string_utils.c headers/string_utils/string_utils.h sources/lexer/parameter_expansion.c headers/lexer/parameter_expansion.h sources/lexer/command_substitution.c headers/lexer/command_substitution.h sources/lexer/arithmetic_expansion.c headers/lexer/arithmetic_expansion.h sources/test.c headers/test.h sources/atom_type.c headers/lexer/candidates.h)
add_executable(OceanDebug sources/impl.c headers/impl.h main.c headers/lexer/token.h sources/lexer/token.c headers/lexer/char_categories.h sources/lexer/char_categories.c headers/eval/string_eval.h sources/eval/string_eval.c headers/combinators/combinator.h sources/combinators/combinator.c sources/combinators/stack.c headers/combinators/stack.h sources/parser/ast.c headers/parser/ast.h sources/parser/cst.c headers/parser/cst.h sources/parser/parse_context.c headers/parser/parse_context.h sources/parser/parser.c headers/parser/parser.h headers/atom_type.h sources/lexer/xbd_operators.c headers/lexer/xbd_operators.h sources/string_utils/string_utils.c headers/string_utils/string_utils.h sources/lexer/parameter_expansion.c headers/lexer/parameter_expansion.h sources/lexer/command_substitution.c headers/lexer/command_substitution.h sources/lexer/arithmetic_expansion.c headers/lexer/arithmetic_expansion.h sources/test.c headers/test.h sources/atom_type.c headers/lexer/candidates.h)
add_executable(OceanTokens main.c sources/impl.c headers/impl.h headers/lexer/token.h sources/lexer/token.c headers/lexer/char_categories.h sources/lexer/char_categories.c headers/eval/string_eval.h sources/eval/string_eval.c headers/combinators/combinator.h sources/combinators/combinator.c sources/combinators/stack.c headers/combinators/stack.h sources/parser/ast.c headers/parser/ast.h sources/parser/cst.c headers/parser/cst.h sources/parser/parse_context.c headers/parser/parse_context.h sources/parser/parser.c headers/parser/parser.h headers/atom_type.h sources/lexer/xbd_operators.c headers/lexer/xbd_operators.h sources/string_utils/string_utils.c headers/string_utils/string_utils.h sources/lexer/parameter_expansion.c headers/lexer/parameter_expansion.h sources/lexer/command_substitution.c headers/lexer/command_substitution.h sources/lexer/arithmetic_expansion.c headers/lexer/arithmetic_expansion.h sources/test.c headers/test.h sources/atom_type.c headers/lexer/candidates.h)

target_compile_definitions(OceanDebug PUBLIC OCEAN_DEBUG_MACRO)
target_compile_definitions(OceanTokens PUBLIC OCEAN_DEBUG_TOKENS_MACRO)

add_test(NAME Tokenization_Simple COMMAND Ocean -t tokenization-simple 0x1)
add_test(NAME Tokenization_3Candi COMMAND Ocean -t tokenization-3-candidates 0x1)
