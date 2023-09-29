#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Types.h"
#include "Stack.h"
#include "Protection.h"
#include "Logfile.h"


int stack_ctor(Stack *stk, int capacity) {

    assert(stk != NULL);
    assert(capacity > 0);

    int needed_capacity = capacity * sizeof(elem_t);

    #ifdef CANARY_MODE
    stk->canary1 = CanaryStack;
    stk->canary2 = CanaryStack;
    needed_capacity += 2 * sizeof(canary_t);
    #endif

    stk->data = (elem_t *)calloc(needed_capacity, sizeof(char));

    if (stk->data == NULL) {
        printf("Sorry! Capacity is too big, there's no enough memory");
    }
    else {

        stk->size = 0;
        stk->capacity = capacity;

        #ifdef CANARY_MODE
        *(canary_t *) stk->data = CanaryData;
        stk->data = (elem_t *)((canary_t *)stk->data + 1);
        *(canary_t *)(stk->data + stk->capacity * sizeof(elem_t)) = CanaryData;
        #endif

        #ifdef HASH_MODE
        stk->hash = stack_calculate(stk);
        #endif
    }

    assert(stk->data != NULL);
    return no_errors;
}


int stack_dtor(Stack *stk) {

    STACK_VERIFY(stk);

    #ifdef CANARY_MODE
    stk->data = stk->data - sizeof(canary_t);
    stk->canary1 = PoisonCanaryValue;
    stk->canary2 = PoisonCanaryValue;
    #endif

    free(stk->data);

    stk->size = PoisonValue;
    stk->capacity = PoisonValue;

    #ifdef HASH_MODE
    stk->hash = PoisonValue;
    #endif

    return no_errors;

}


int stack_push(Stack *stk, elem_t value) {

    STACK_VERIFY(stk);

    if (stk->size == stk->capacity) {
        stack_realloc(stk, ReallocCoeff*stk->capacity);
    }

    stk->data[stk->size] = value;
    stk->size ++;

    #ifdef HASH_MODE
    stk->hash = stack_calculate(stk);
    #endif

    STACK_VERIFY(stk);

    return no_errors;
}


int stack_pop(Stack *stk, elem_t *retvalue) {

    STACK_VERIFY(stk);

    if ((stk->size - 1) == (stk->capacity)/ReallocCoeff) {
        stack_realloc(stk, stk->capacity/ReallocCoeff);
    }

    stk->size --;
    *retvalue = stk->data[stk->size];
    stk->data[stk->size] = PoisonValue;

    #ifdef HASH_MODE
    stk->hash = stack_calculate(stk);
    #endif

    STACK_VERIFY(stk);

    return no_errors;
}


int stack_realloc(Stack *stk, int newcapacity) {

    STACK_VERIFY(stk);

    int needed_capacity = newcapacity * sizeof(elem_t);

    #ifdef CANARY_MODE
    needed_capacity += 2 * sizeof(canary_t);
    *(elem_t *)(stk->data + stk->capacity * sizeof(elem_t)) = PoisonValue;
    stk->data = (elem_t *)((char *)stk->data - sizeof(canary_t));
    #endif

    elem_t *temp_data = (elem_t *)realloc(stk->data, needed_capacity);

    if (temp_data != NULL) {

        stk->capacity = newcapacity;
        stk->data = temp_data;

        #ifdef CANARY_MODE
        stk->data = (elem_t *)((canary_t *)stk->data + 1);
        *(canary_t *)(stk->data + stk->capacity * sizeof(elem_t)) = CanaryData;
        #endif

    }
    else {
        printf("Sorry! Capacity is too big, there's no enough memory");
    }
    return no_errors;
}


void stack_dump(const struct Stack *stk, const char *file, int line, const char *function) {

    printf("stack_dump from file: %s line %d function: %s stack: %s\n\n",
                                        file, line, function, stk->name);

    #ifdef CANARY_MODE
    printf("struct beginning canary = %X\n", stk->canary1);
    printf("struct end canary = %X\n\n", stk->canary2);
    printf("data beginning canary = %X\n", *(canary_t *)(stk->data - 1));
    printf("data end canary = %X\n\n\n", *(canary_t *)(stk->data + stk->capacity * sizeof(elem_t)));
    #endif

    #ifdef HASH_MODE
    printf("hash = " ELEMF "\n\n", stk->hash);
    #endif

    printf("size = %d\n", stk->size);
    printf("capacity = %d\n", stk->capacity);

    for (int i = 0; i < stk->size; i++) {
        printf("*[%d] = " ELEMF "\n", i, stk->data[i]);
    }

    for (int i = stk->size; i < stk->capacity; i++) {
        printf("[%d] = POISONED\n", i);
    }

}






