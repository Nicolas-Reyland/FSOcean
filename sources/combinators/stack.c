//
// Created on 12/05/2022.
//

#include <stdlib.h>
#include <stdio.h>
#include "combinators/stack.h"

static void stack_push(struct Stack * stack, int value)
{
    struct StackNode * new_head = malloc(sizeof(struct StackNode));
    new_head->value = value;
    new_head->next = stack->head;
    stack->head = new_head;
    stack->size++;
}

static int stack_pop(struct Stack * stack)
{
    if (stack->head == NULL) {
        fprintf(stderr, "Tried to pop empty value stack");
        exit(1);
    }
    // local ref to head
    struct StackNode * head = stack->head;
    // re-define head in stack
    stack->head = head->next;
    stack->size--;
    // copy value of previous head into local variable
    int value = head->value;
    // free old head
    free(head);
    // return copy of old head value
    return value;
}

struct Stack create_stack()
{
    return (struct Stack) {
            .size = 0,
            .head = NULL,
            .push = stack_push,
            .pop  = stack_pop,
    };
}
