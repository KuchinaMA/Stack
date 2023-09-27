#include <stdio.h>

#include "Types.h"
#include "Stack.h"
#include "Logfile.h"

int main() {

    open_log("Stackerrors.log", __TIME__);

    STACK_CONSTRUCT(stk, StackCapacity);

    PRINT_STACK(&stk);

    for (elem_t i = 0; i < 6; i++) {
        stack_push(&stk, i);
        PRINT_STACK(&stk);
    }

    printf(ELEMF"\n", stk.data[0]);

    stk.data[0] = 1;

    elem_t x = 0;

    for (int i = 0; i < 3; i++) {
        x = stack_pop(&stk);
        PRINT_STACK(&stk);
    }

    //stk.size = 5;
    //stk.capacity = 3;


    stack_dtor(&stk);

    return 0;
}

