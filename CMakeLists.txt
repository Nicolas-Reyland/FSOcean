cmake_minimum_required(VERSION 3.21)
project(Ocean C)

set(CMAKE_C_STANDARD 11)

include(CTest)

include_directories(headers)

add_executable(Ocean main.c sources/misc/impl.c headers/misc/impl.h headers/lexer/token.h sources/lexer/token.c headers/lexer/char_categories.h sources/lexer/char_categories.c headers/eval/string_eval.h sources/eval/string_eval.c headers/combinators/combinator.h sources/combinators/combinator.c sources/misc/stack.c headers/misc/stack.h sources/parser/ast.c headers/parser/ast.h sources/parser/cst.c headers/parser/cst.h sources/parser/parse_context.c headers/parser/parse_context.h sources/parser/parser.c headers/parser/parser.h headers/misc/atom_type.h sources/lexer/xbd_operators.c headers/lexer/xbd_operators.h sources/string_utils/string_utils.c headers/string_utils/string_utils.h sources/lexer/parameter_expansion.c headers/lexer/parameter_expansion.h sources/lexer/command_substitution.c headers/lexer/command_substitution.h sources/lexer/arithmetic_expansion.c headers/lexer/arithmetic_expansion.h sources/testing/test.c headers/testing/test.h sources/misc/atom_type.c headers/lexer/candidates.h sources/misc/interactive.c headers/misc/interactive.h sources/lexer/shell_grammar/lexical_conventions.c headers/lexer/shell_grammar/lexical_conventions.h headers/combinators/combinator_types.h headers/lexer/shell_grammar/rules.h sources/lexer/shell_grammar/rules.c sources/misc/output.c headers/misc/output.h sources/eval/expansion/filename_exp.c headers/eval/expansion/filename_exp.h)
add_executable(OceanDebug sources/misc/impl.c headers/misc/impl.h main.c headers/lexer/token.h sources/lexer/token.c headers/lexer/char_categories.h sources/lexer/char_categories.c headers/eval/string_eval.h sources/eval/string_eval.c headers/combinators/combinator.h sources/combinators/combinator.c sources/misc/stack.c headers/misc/stack.h sources/parser/ast.c headers/parser/ast.h sources/parser/cst.c headers/parser/cst.h sources/parser/parse_context.c headers/parser/parse_context.h sources/parser/parser.c headers/parser/parser.h headers/misc/atom_type.h sources/lexer/xbd_operators.c headers/lexer/xbd_operators.h sources/string_utils/string_utils.c headers/string_utils/string_utils.h sources/lexer/parameter_expansion.c headers/lexer/parameter_expansion.h sources/lexer/command_substitution.c headers/lexer/command_substitution.h sources/lexer/arithmetic_expansion.c headers/lexer/arithmetic_expansion.h sources/testing/test.c headers/testing/test.h sources/misc/atom_type.c headers/lexer/candidates.h sources/misc/interactive.c headers/misc/interactive.h sources/lexer/shell_grammar/lexical_conventions.c headers/lexer/shell_grammar/lexical_conventions.h headers/combinators/combinator_types.h headers/lexer/shell_grammar/rules.h sources/lexer/shell_grammar/rules.c sources/misc/output.c headers/misc/output.h sources/eval/expansion/filename_exp.c headers/eval/expansion/filename_exp.h)
add_executable(OceanTokens main.c sources/misc/impl.c headers/misc/impl.h headers/lexer/token.h sources/lexer/token.c headers/lexer/char_categories.h sources/lexer/char_categories.c headers/eval/string_eval.h sources/eval/string_eval.c headers/combinators/combinator.h sources/combinators/combinator.c sources/misc/stack.c headers/misc/stack.h sources/parser/ast.c headers/parser/ast.h sources/parser/cst.c headers/parser/cst.h sources/parser/parse_context.c headers/parser/parse_context.h sources/parser/parser.c headers/parser/parser.h headers/misc/atom_type.h sources/lexer/xbd_operators.c headers/lexer/xbd_operators.h sources/string_utils/string_utils.c headers/string_utils/string_utils.h sources/lexer/parameter_expansion.c headers/lexer/parameter_expansion.h sources/lexer/command_substitution.c headers/lexer/command_substitution.h sources/lexer/arithmetic_expansion.c headers/lexer/arithmetic_expansion.h sources/testing/test.c headers/testing/test.h sources/misc/atom_type.c headers/lexer/candidates.h sources/misc/interactive.c headers/misc/interactive.h sources/lexer/shell_grammar/lexical_conventions.c headers/lexer/shell_grammar/lexical_conventions.h headers/combinators/combinator_types.h headers/lexer/shell_grammar/rules.h sources/lexer/shell_grammar/rules.c sources/misc/output.c headers/misc/output.h sources/eval/expansion/filename_exp.c headers/eval/expansion/filename_exp.h)

target_compile_definitions(OceanDebug PUBLIC OCEAN_DEBUG_MACRO)
target_compile_definitions(OceanTokens PUBLIC OCEAN_DEBUG_TOKENS_MACRO)

# Tokenization tests
add_test(NAME Tokenization_Simple COMMAND Ocean -t tokenization/simple 0x1)
add_test(NAME Tokenization_3Candi COMMAND Ocean -t tokenization/3-candidates 0x1)
# Parsing tests
add_test(NAME Parsing_Simple COMMAND Ocean -t parsing/simple 0x10)
add_test(NAME Parsing_And_Or COMMAND Ocean -t parsing/and_or 0x10)
add_test(NAME Parsing_Redirect COMMAND Ocean -t parsing/redirect 0x10)

# Abstraction tests

# Executions tests
