#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Types.h"
#include "Stack.h"
#include "Protection.h"
#include "Logfile.h"

#define STACK_VERIFY(stk) {int err = stack_verify(stk); \
            if (err > 0) { \
            print_errors(stk, err); \
            stack_dump_err((stk), __FILE__, __LINE__, __func__, LOG_FILE); }\
}


int stack_ctor(Stack *stk, int capacity) {

    assert(stk != NULL);
    //assert(capacity > 0);

    #ifdef CANARY_MODE
    stk->canary1 = CanaryStack;
    stk->canary2 = CanaryStack;
    stk->data = (elem_t *) calloc(capacity * sizeof(elem_t) + 2 * sizeof(canary_t), sizeof(char));
    #endif

    #ifndef CANARY_MODE
    stk->data = (elem_t *)calloc(capacity, sizeof(elem_t));
    #endif


    if (stk->data == NULL) {
        printf("Sorry! Capacity is too big, there's no enough memory");
    }
    else {

        stk->size = 0;
        stk->capacity = capacity;

        #ifdef CANARY_MODE
        *(canary_t *) stk->data = CanaryBuf;
        stk->data = (elem_t *)((canary_t *)stk->data + 1);
        *(canary_t *)(stk->data + stk->capacity * sizeof(elem_t)) = CanaryBuf;
        #endif

        #ifdef HASH_MODE
        stk->hash = stack_calculate(stk);
        #endif
    }

    assert(stk->data != NULL);

}


int stack_dtor(Stack *stk) {

    STACK_VERIFY(stk);

    #ifdef CANARY_MODE
    free((canary_t *)stk->data - 1);
    stk->canary1 = PoisonCanaryValue;
    stk->canary2 = PoisonCanaryValue;
    #endif

    #ifndef CANARY_MODE
    free(stk->data);
    #endif

    stk->size = PoisonValue;
    stk->capacity = PoisonValue;

    #ifdef HASH_MODE
    stk->hash = PoisonValue;
    #endif

    return 0;

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

    return 0;
}


elem_t stack_pop(Stack *stk, elem_t *retvalue) {

    STACK_VERIFY(stk);

    stk->size --;
    *retvalue = stk->data[stk->size];
    stk->data[stk->size] = PoisonValue;

    if (stk->size == (stk->capacity)/ReallocCoeff) {
        stack_realloc(stk, stk->capacity/ReallocCoeff);
    }

    #ifdef HASH_MODE
    stk->hash = stack_calculate(stk);
    #endif

    STACK_VERIFY(stk);

    return 0;
}


void stack_realloc(Stack *stk, int newcapacity) {

    #ifdef HASH_MODE
    stk->hash = stack_calculate(stk);
    #endif
    STACK_VERIFY(stk);

    #ifdef CANARY_MODE
    canary_t temp_canary = *(canary_t *)(stk->data + stk->capacity * sizeof(elem_t));
    *(elem_t *)(stk->data + stk->capacity * sizeof(elem_t)) = PoisonValue;

    stk->data = (elem_t *)((char *)stk->data - sizeof(canary_t));
    elem_t *temp_data = (elem_t *)realloc(stk->data, newcapacity * sizeof(elem_t) + 2 * sizeof(canary_t));
    #endif

    #ifndef CANARY_MODE
    elem_t *temp_data = (elem_t *)realloc(stk->data, newcapacity * sizeof(elem_t));

    #endif

    if (temp_data != NULL) {

        stk->capacity = newcapacity;

        #ifdef CANARY_MODE
        stk->data = (elem_t *)((char *)temp_data + sizeof(canary_t));
        *(canary_t *)(stk->data + stk->capacity * sizeof(elem_t)) = temp_canary;
        #endif

        #ifndef CANARY_MODE
        stk->data = temp_data;
        #endif
    }
    else {
        printf("Sorry! Capacity is too big, there's no enough memory");
    }
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






