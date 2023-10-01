#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Types.h"
#include "Stack.h"
#include "Protection.h"
#include "Logfile.h"


int stack_ctor(Stack *stk, int capacity) {

    assert(stk != NULL);

    int needed_capacity = capacity * sizeof(elem_t);


    USE_CANARIES(stk->canary1 = CanaryStack;
    stk->canary2 = CanaryStack;
    needed_capacity += 2 * sizeof(canary_t);)


    stk->data = (elem_t *)calloc(needed_capacity, sizeof(char));

    if (stk->data == NULL) {
        printf("Sorry! Capacity is too big, there's no enough memory");
        fprintf(LOG_FILE, "In function: %s ERROR! Pointer to stk.data is NULL\n\n", __func__);

        return DataNull;
    }
    else {

        stk->size = 0;
        stk->capacity = capacity;


        USE_CANARIES(*(canary_t *) stk->data = CanaryData;
        stk->data = (elem_t *)((canary_t *)stk->data + 1);
        *(canary_t *)(stk->data + stk->capacity * sizeof(elem_t)) = CanaryData;)


        USE_HASH(stk->data_hash = data_hash_calculate(stk);
                 stk->struct_hash = struct_hash_calculate(stk);)
    }

    return NoErrors;
}


int stack_dtor(Stack *stk) {

    STACK_VERIFY(stk);


    USE_CANARIES(stk->data = stk->data - sizeof(canary_t);
    stk->canary1 = PoisonCanaryValue;
    stk->canary2 = PoisonCanaryValue;)


    free(stk->data);

    stk->size = PoisonValue;
    stk->capacity = PoisonValue;


    USE_HASH(stk->data_hash = PoisonValue;
             stk->struct_hash = PoisonValue;)

    return NoErrors;

}


int stack_push(Stack *stk, elem_t value) {

    STACK_VERIFY(stk);

    if (stk->size >= stk->capacity) {
        stack_realloc(stk, ReallocCoeff*stk->capacity);
    }

    stk->data[stk->size] = value;
    stk->size ++;


    USE_HASH(stk->data_hash = data_hash_calculate(stk);
             stk->struct_hash = struct_hash_calculate(stk);)

    STACK_VERIFY(stk);

    return NoErrors;
}


int stack_pop(Stack *stk, elem_t *retvalue) {

    STACK_VERIFY(stk);

    if ((stk->size - 1) < (stk->capacity)/ReallocCoeff) {
        stack_realloc(stk, stk->capacity/ReallocCoeff);
    }

    stk->size --;
    *retvalue = stk->data[stk->size];
    stk->data[stk->size] = PoisonValue;


    USE_HASH(stk->data_hash = data_hash_calculate(stk);
             stk->struct_hash = struct_hash_calculate(stk);)

    STACK_VERIFY(stk);

    return NoErrors;
}


int stack_realloc(Stack *stk, int newcapacity) {

    STACK_VERIFY(stk);

    int needed_capacity = newcapacity * sizeof(elem_t);


    USE_CANARIES(needed_capacity += 2 * sizeof(canary_t);
    *(elem_t *)(stk->data + stk->capacity * sizeof(elem_t)) = PoisonValue;
    stk->data = (elem_t *)((char *)stk->data - sizeof(canary_t));)

    elem_t *temp_data = (elem_t *)realloc(stk->data, needed_capacity);

    if (temp_data != NULL) {

        stk->capacity = newcapacity;
        stk->data = temp_data;


        USE_CANARIES(stk->data = (elem_t *)((canary_t *)stk->data + 1);
        *(canary_t *)(stk->data + stk->capacity * sizeof(elem_t)) = CanaryData;)

    }
    else {
        printf("Sorry! Capacity is too big, there's no enough memory");
        fprintf(LOG_FILE, "In function: %s ERROR! Pointer to stk.data is NULL\n\n", __func__);

        return DataNull;
    }

    USE_HASH(stk->data_hash = data_hash_calculate(stk);
             stk->struct_hash = struct_hash_calculate(stk);)

    STACK_VERIFY(stk);

    return NoErrors;
}

void stack_dump(const struct Stack *stk, const char *file, int line, const char *function, FILE* fp) {

    fprintf(fp, "stack_dump from file: %s line %d function: %s stack: %s\n\n",
                                             file, line, function, stk->name);


    USE_CANARIES(fprintf(fp, "struct beginning canary = %X\n", stk->canary1);
    fprintf(fp, "struct end canary = %X\n\n", stk->canary2);
    fprintf(fp, "data beginning canary = %X\n", *(canary_t *)(stk->data - 1));
    fprintf(fp, "data end canary = %X\n\n\n", *(canary_t *)(stk->data +
                                            stk->capacity * sizeof(elem_t)));)

    USE_HASH(fprintf(fp, "data hash = " ELEMF "\n\n", stk->data_hash);
             fprintf(fp, "struct hash = " ELEMF "\n\n", stk->struct_hash);)

    fprintf(fp, "size = %d\n", stk->size);
    fprintf(fp, "capacity = %d\n", stk->capacity);

    for (int i = 0; i < stk->size; i++) {
        fprintf(fp, "*[%d] = " ELEMF "\n", i, stk->data[i]);
    }

    for (int i = stk->size; i < stk->capacity; i++) {
        fprintf(fp, "[%d] = POISONED\n", i);
    }

}






