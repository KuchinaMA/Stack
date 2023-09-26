#include <stdio.h>

#include "Types.h"
#include "Stack.h"
#include "Logfile.h"

/*#define STACK_CONSTRUCT(var, elements_quantity) \
struct Stack var = {};                          \
stack_ctor(&var, elements_quantity);            \
var.var_name = #var; */

int main() {

    open_log("Stackerrors.log");

    //fopen(LOG_FILE);

    struct Stack stk = {};

    stack_ctor(&stk, StackCapacity);
    //stk.var_name = "stk";

    PRINT_STACK(&stk);


    for (int i = 0; i < 8; i++) {
        stack_push(&stk, i);
        PRINT_STACK(&stk);
    }

    stk.size = 5;
    stk.capacity = 3;

    elem_t x = 0;

    for (int i = 0; i < 1; i++) {
        x = stack_pop(&stk);
        PRINT_STACK(&stk);
    }

    stack_dtor(&stk);

    return 0;
}

