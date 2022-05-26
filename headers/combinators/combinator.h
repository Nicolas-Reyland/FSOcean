//
// Created on 12/05/2022.
//

#ifndef OCEAN_COMBINATOR_H
#define OCEAN_COMBINATOR_H

#define COMBINATOR_NONE_TYPE 0x0

#define COMBINATOR_INVERTED_TYPE 0xe0
#define COMBINATOR_SEQUENCE_TYPE 0xe1
#define COMBINATOR_SEQUENCE_UNIT_TYPE 0xe2
#define COMBINATOR_REPETITION_TYPE 0xe3
#define COMBINATOR_CHOICE_TYPE 0xe4
#define COMBINATOR_OPTIONAL_TYPE 0xe5

#define COMBINATOR_SEPARATED_TYPE 0xe6
#define COMBINATOR_SEPARATED_REPETITION_TYPE 0xe7

#define COMBINATOR_LOOKAHEAD_TYPE 0xfe
#define COMBINATOR_GENERATOR_TYPE 0xff

#include <stdbool.h>

typedef struct Combinator Combinator;
struct Combinator;

typedef bool (*cmb_exec_function)(void *, struct Combinator *);
typedef void (*cmb_commit_function)(void *, struct Combinator *, void *, void *, int);

struct Combinator {
    int type;
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
