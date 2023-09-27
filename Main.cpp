#include <stdio.h>

#include "Types.h"
#include "Stack.h"
#include "Logfile.h"

int main() {

    open_log("Stackerrors.log", __TIME__);

    STACK_CONSTRUCT(stk, StackCapacity);

    PRINT_STACK(&stk);

    for (elem_t i = 0; i < 8; i++) {
        stack_push(&stk, i);
        PRINT_STACK(&stk);
    }

    elem_t x = 0;

    for (int i = 0; i < 3; i++) {
        x = stack_pop(&stk);
        PRINT_STACK(&stk);
    }

    //stk.size = 5;
    //stk.capacity = 3;

    for (int i = 0; i < 1; i++) {
        x = stack_pop(&stk);
        PRINT_STACK(&stk);
    }

    stack_dtor(&stk);

    return 0;
}

