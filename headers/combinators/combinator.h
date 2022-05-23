//
// Created on 12/05/2022.
//

#ifndef OCEAN_COMBINATOR_H
#define OCEAN_COMBINATOR_H

typedef struct Combinator Combinator;
struct Combinator;

#include <stdbool.h>
#include "lexer/token.h"
#include "stack.h"
#include "parser/cst.h"

typedef bool (*cmb_exec_function)(void *, struct Combinator *);

struct Combinator {
    int type;
    struct Combinator * sub_combinators;
    size_t num_sub_combinators;
    cmb_exec_function decorator;
    cmb_exec_function exec;
    cmb_exec_function exec_f;
    void (*commit)(void *, struct Combinator *, void *, void *, int);
    // for forward-referencing
    struct Combinator (*cmb_generator)(void);
};

struct Combinator cmb_create(
        cmb_exec_function exec,
        cmb_exec_function exec_f,
        void (*commit)(void *, struct Combinator *, void *, void *, int)
);
bool execute_cmb(void * ctx, Combinator * p);

Combinator cmb_forward_ref(cmb_exec_function cmb_exec, struct Combinator (*cmb_generator)(void));

Combinator cmb_inverted(cmb_exec_function cmb_exec, Combinator p);
Combinator cmb_sequence(cmb_exec_function cmb_exec, unsigned int, ...);
Combinator cmb_repetition(Combinator);
Combinator cmb_optional(Combinator);
Combinator cmb_choice(cmb_exec_function cmb_exec, unsigned int, ...);
Combinator cmb_separated(cmb_exec_function cmb_exec, Combinator p, Combinator separator);
Combinator cmb_lookahead(cmb_exec_function cmb_exec, Combinator p);

Combinator typed_cmb(Combinator, int);

#endif // OCEAN_COMBINATOR_H
