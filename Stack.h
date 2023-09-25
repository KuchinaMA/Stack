#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

#include <stdio.h>

#define PRINT_STACK(stk) stack_dump((stk), __FILE__, __LINE__, __func__);

struct Stack {
    canary_t canary1;
    elem_t *data;
    int size;
    int capacity;
    elem_t hash;
    canary_t canary2;
};

struct Errors {
    int stack_null;
    int data_null;
    int negative_size;
    int negative_capacity;
    int small_capacity;
    int incorrect_canary;
    int incorrect_hash;
};

const int StackCapacity = 5;
const int Coeff = 2;
const canary_t CanaryStack = 0xDEADBEEF;
const elem_t CanaryBuf = 0xBADCAFE;


int stack_ctor(Stack *stk, int capacity);
int stack_dtor(Stack *stk);
int stack_push(Stack *stk, elem_t value);
elem_t stack_pop(Stack *stk);
void stack_realloc(Stack *stk, int newsize);
int stack_verify (const struct Stack *stk, struct Errors *err);
void stack_dump(const struct Stack *stk, const char *file, int line, const char *function);
hash_t stack_calculate(const struct Stack *stk);
void print_errors(const struct Errors *err);


#endif // STACK_H_INCLUDED
