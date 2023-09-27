#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

#include <stdio.h>

#define STACK_CONSTRUCT(var, elements_quantity) \
struct Stack var = {};                          \
stack_ctor(&var, elements_quantity);            \
var.name = #var;

#define PRINT_STACK(stk) stack_dump((stk), __FILE__, __LINE__, __func__);

struct Stack {
    canary_t canary1;
    elem_t *data;
    int size;
    int capacity;
    const char *name;
    elem_t hash;
    canary_t canary2;
};

enum Errors {
    stack_null        = 1,
    data_null         = 1 << 1,
    negative_size     = 1 << 2,
    negative_capacity = 1 << 3,
    small_capacity    = 1 << 4,
    incorrect_canary  = 1 << 5,
    incorrect_hash    = 1 << 6
};

const int StackCapacity = 5;
const int Coeff = 2;

const canary_t CanaryStack = 0xDEADBEEF;
const elem_t CanaryBuf = 0xBADCAFE;


int stack_ctor(Stack *stk, int capacity);
int stack_dtor(Stack *stk);

int stack_push(Stack *stk, elem_t value);
elem_t stack_pop(Stack *stk);

void stack_realloc(Stack *stk, int newcapacity);

int stack_verify (const struct Stack *stk);
void stack_dump(const struct Stack *stk, const char *file, int line, const char *function);
hash_t stack_calculate(const struct Stack *stk);
void print_errors(const struct Stack *stk, int err);
void stack_dump_err(const struct Stack *stk, const char *file, int line, const char *function, FILE* fp);


#endif // STACK_H_INCLUDED
