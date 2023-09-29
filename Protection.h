#ifndef PROTECTION_H_INCLUDED
#define PROTECTION_H_INCLUDED

#include <stdio.h>
#include "Types.h"

//#define CANARY_MODE
//#define HASH_MODE

/*struct Stack {

    #ifdef CANARY_MODE
    canary_t canary1;
    #endif

    elem_t *data;
    int size;
    int capacity;
    const char *name;

    #ifdef HASH_MODE
    elem_t hash;
    #endif

    #ifdef CANARY_MODE
    canary_t canary2;
    #endif
};*/

enum Errors {
    stack_null        = 1,
    data_null         = 1 << 1,
    negative_size     = 1 << 2,
    negative_capacity = 1 << 3,
    small_capacity    = 1 << 4,
    incorrect_canary  = 1 << 5,
    incorrect_hash    = 1 << 6
};

int stack_verify (const struct Stack *stk);

hash_t stack_calculate(const struct Stack *stk);

void print_errors(const struct Stack *stk, int err);
void stack_dump_err(const struct Stack *stk, const char *file, int line,
                                         const char *function, FILE* fp);


#endif // PROTECTION_H_INCLUDED
