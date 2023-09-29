#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

#include <stdio.h>

#define STACK_CONSTRUCT(var, elements_quantity) \
struct Stack var = {};                          \
stack_ctor(&var, elements_quantity);            \
var.name = #var;

#define PRINT_STACK(stk) stack_dump((stk), __FILE__, __LINE__, __func__);

#define CANARY_MODE
#define HASH_MODE

struct Stack {

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
};


const int StackDefaultCapacity = 5;
const int ReallocCoeff = 2;

const canary_t CanaryStack = 0xDEADBEEF;
const canary_t CanaryBuf = 0xBADCAFE;


int stack_ctor(Stack *stk, int capacity);
int stack_dtor(Stack *stk);

int stack_push(Stack *stk, elem_t value);
elem_t stack_pop(Stack *stk, elem_t *retvalue);

void stack_realloc(Stack *stk, int newcapacity);

void stack_dump(const struct Stack *stk, const char *file, int line, const char *function);


#endif // STACK_H_INCLUDED
