//
// Created by Nicolas Reyland on 5/27/22.
//

#ifndef OCEAN_RULES_H
#define OCEAN_RULES_H

#include <stddef.h>
#include "lexer/token.h"
#include "parser/cst.h"

CSTNode * shell_grammar_rules(Token * tokens, size_t num_tokens, size_t * num_nodes);

#endif // OCEAN_RULES_H
