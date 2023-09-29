#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

#include <stdio.h>

#define STACK_CONSTRUCT(var, elements_quantity) \
struct Stack var = {};                          \
stack_ctor(&var, elements_quantity);            \
var.name = #var;

#define PRINT_STACK(stk) stack_dump((stk), __FILE__, __LINE__, __func__);


#define CANARY_MODE

#ifdef CANARY_MODE
#define USE_CANARIES(lines) \
lines;
#else
#define USE_CANARIES(lines) \
;
#endif


#define HASH_MODE

#ifdef HASH_MODE
#define USE_HASH(lines) \
lines;
#else
#define USE_HASH(lines) \
;
#endif


/**Structure with all information about stack:
 * array with data
 * size of space occupied by data
 * total amount of space in stack
 * name of stack
 * protecting values: canaries and value of hash if there is relevant mode
*/
struct Stack {

    USE_CANARIES(canary_t canary1;)

    elem_t *data;
    int size;
    int capacity;
    const char *name;

    USE_HASH(elem_t hash;)

    USE_CANARIES(canary_t canary2;)
};

/// Capacity that stack will have initially
const int StackDefaultCapacity = 5;
/** Number of times in which stack capacity increases or decreases when
 *  boundary values of size are reached
*/
const int ReallocCoeff = 2;

/// Value of canary that protects structure with info about stack
const canary_t CanaryStack = 0xDEADBEEF;
/// Value of canary tat protects stack data
const canary_t CanaryData = 0xBADCAFE;

/** Initialize the stack structure
 *  \param [in] stk pointer to structure with stack
 *  \paran [in] capacity original capacity of stack
*/
int stack_ctor(Stack *stk, int capacity);
/** Destructs stack structure before completion of the program
 *  \param [in] stk pointer to structure with stack
*/
int stack_dtor(Stack *stk);

/** Puts an element in stack
 *  \param [in] stk pointer to structure with stack
 *  \param [in] value of an element that will be put in stack
*/
int stack_push(Stack *stk, elem_t value);
/** Gets an element from stack
 *  \param [in] stk pointer to structure with stack
 *  \param [out] retvalue value of an element that will be gotten from stack
*/
int stack_pop(Stack *stk, elem_t *retvalue);

/** Finds a new place for stack data if capacity of stack is changed
 *  \param [in] stk pointer to structure with stack
 *  \param [in] newcapacity new value of stack capacity
*/
int stack_realloc(Stack *stk, int newcapacity);

/** Prints all information about stack
 * \param [in] stk pointer to structure with stack
 * \param [in] file name of file from which stack is printed
 * \param [in] line number of line in code from which stack is printed
 * \param [in] function name of function from which stack is printed
*/
void stack_dump(const struct Stack *stk, const char *file, int line, const char *function);


#endif // STACK_H_INCLUDED
