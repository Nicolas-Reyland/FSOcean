//
// Created on 16/05/2022.
//

#include "parser/cst.h"

const char * const CONCRETE_NODE_TYPE_STRING[] = {
        [(ConcreteNodeType) NONE] = "none",
        [(ConcreteNodeType) COMMAND_ALL] = "command_all",
        [(ConcreteNodeType) COMMAND] = "command",
        [(ConcreteNodeType) COMMAND_TAIL] = "command_tail",
        [(ConcreteNodeType) NAME] = "name",
        [(ConcreteNodeType) NAMES] = "names",
        [(ConcreteNodeType) REDIRECT] = "redirect",
        [(ConcreteNodeType) REDIRECT_IN] = "redirect_in",
        [(ConcreteNodeType) REDIRECT_OUT] = "redirect_out",
        [(ConcreteNodeType) CMD_SEP] = "cmd_sep",
        [(ConcreteNodeType) PIPE] = "pipe",
        [(ConcreteNodeType) AMP] = "amp",
        [(ConcreteNodeType) NEW_CMD] = "new_cmd",
        [(ConcreteNodeType) LITERAL] = "literal",
        [(ConcreteNodeType) DOUBLEQ] = "doubleq",
        [(ConcreteNodeType) SINGLEQ] = "singleq",
        [(ConcreteNodeType) SEQUENCE] = "sequence",
        [(ConcreteNodeType) REPETITION] = "repetition",
        [(ConcreteNodeType) CHOICE] = "choice",
        [(ConcreteNodeType) OPTIONAL] = "optional",
};
