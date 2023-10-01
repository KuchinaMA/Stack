#ifndef PROTECTION_H_INCLUDED
#define PROTECTION_H_INCLUDED

#include <stdio.h>
#include "Types.h"
#include "Stack.h"

#define STACK_VERIFY(stk) {int err = stack_verify(stk); \
            if (err > 0) { \
                print_errors(stk, err); \
                stack_dump((stk), __FILE__, __LINE__, __func__, LOG_FILE);  \
            return err; } \
}

///Constants that allow to find errors
enum Errors {
    no_errors         = 0,
    stack_null        = 1,
    data_null         = 1 << 1,
    negative_size     = 1 << 2,
    negative_capacity = 1 << 3,
    small_capacity    = 1 << 4,
    incorrect_canary  = 1 << 5,
    incorrect_hash    = 1 << 6
};

/** Checks if there are errors in stack
 * \param [in] stk pointer to structure with stack
 * \returns a number that allows definetly find all errors if there are any
*/
int stack_verify (const struct Stack *stk);

/** Hash function for stack
 * \param [in] stk pointer to structure with stack
 * \returns hash value
*/
hash_t stack_calculate(const struct Stack *stk);

/** Prints messages about errors in stack if there are any
 * \param [in] stk pointer to structure with stack
 * \param [in] err a number that allows to identify all errors
*/
void print_errors(const struct Stack *stk, int err);


#endif // PROTECTION_H_INCLUDED
