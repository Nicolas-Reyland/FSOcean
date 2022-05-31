//
// Created on 12/05/2022.
//

#ifndef OCEAN_COMBINATOR_H
#define OCEAN_COMBINATOR_H

#include <stdbool.h>
#include "atom_type.h"

typedef struct Combinator Combinator;
struct Combinator;

typedef bool (*cmb_exec_function)(void *, struct Combinator *);
typedef void (*cmb_commit_function)(void *, struct Combinator *, void *, void *, int);

struct Combinator {
    ParserType type;
    struct Combinator * sub_combinators;
    size_t num_sub_combinators;
    cmb_exec_function decorator;
    cmb_exec_function exec;
    cmb_exec_function exec_f;
    cmb_commit_function commit;
    // for forward-referencing
    struct Combinator (*cmb_generator)(void);
};

struct Combinator cmb_create(
        cmb_exec_function exec,
        cmb_exec_function exec_f,
        cmb_commit_function commit
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
