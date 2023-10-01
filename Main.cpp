#include <stdio.h>

#include "Types.h"
#include "Stack.h"
#include "Logfile.h"
#include "Protection.h"

int main() {

    open_log("Stackerrors.log", __TIME__);

    STACK_CONSTRUCT(stk, StackDefaultCapacity);

    PRINT_STACK(&stk);

    for (elem_t i = 0; i < 3; i++) {
        int x = stack_push(&stk, i);
        PRINT_STACK(&stk);
    }

    //printf(ELEMF"\n", stk.data[0]);

    //stk.data[0] = 1000;

    //stk.capacity = 3;
    //stk.size = 4;

    //stk.name = "aboba";

    /*elem_t x = 0;

    for (int i = 0; i < 3; i++) {
        int t = stack_pop(&stk, &x);
        PRINT_STACK(&stk);
    } */

    stack_dtor(&stk);

    return 0;
}

