#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#define CANARY_MODE
#define HASH_MODE

#define STACK_VERIFY(stk) {int err = stack_verify(stk); \
            if (err > 0) { \
            print_errors(stk, err); \
            stack_dump_err((stk), __FILE__, __LINE__, __func__, LOG_FILE); }\
}


#include "Types.h"
#include "Stack.h"
#include "Logfile.h"

int stack_ctor(Stack *stk, int capacity) {

    assert(stk != NULL);
    assert(capacity > 0);

    #ifdef CANARY_MODE
    stk->canary1 = CanaryStack;
    stk->canary2 = CanaryStack;
    #endif

    #ifdef CANARY_MODE
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

    free((canary_t *)stk->data - 1);
    stk->size = PoisonValue;
    stk->capacity = PoisonValue;
    stk->hash = PoisonValue;

    return 0;

}


int stack_push(Stack *stk, elem_t value) {

    STACK_VERIFY(stk);

    if (stk->size == stk->capacity) {
        stack_realloc(stk, Coeff*stk->capacity);
    }

    stk->data[stk->size] = value;
    stk->size ++;
    stk->hash = stack_calculate(stk);

    STACK_VERIFY(stk);

    return 0;
}


elem_t stack_pop(Stack *stk) {

    STACK_VERIFY(stk);

    stk->size --;
    elem_t ans = stk->data[stk->size];
    stk->data[stk->size] = PoisonValue;

    stk->hash = stack_calculate(stk);

    if (stk->size == (stk->capacity)/Coeff) {
        stack_realloc(stk, stk->capacity/Coeff);
    }

    stk->hash = stack_calculate(stk);

    STACK_VERIFY(stk);

    return ans;
}


void stack_realloc(Stack *stk, int newcapacity) {

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


hash_t stack_calculate(const struct Stack *stk) {

    hash_t sum = 0;

    for (int i = 0; i < stk->size; i++) {
        sum += (hash_t)stk->data[i];
    }

    sum += (hash_t)stk->size;
    sum += (hash_t)stk->capacity;

    return sum;
}


int stack_verify (const struct Stack *stk) {

    int ans = 0;

    if (stk == NULL)                       ans = ans | stack_null;

    if (stk->data == NULL)                 ans = ans | data_null;

    if (stk->size < 0)                     ans = ans | negative_size;

    if (stk->capacity < 0)                 ans = ans | negative_capacity;

    if (stk->capacity < stk->size)         ans = ans | small_capacity;

    #ifdef CANARY_MODE
    if (stk->canary1 != CanaryStack || stk->canary2 != CanaryStack ||
        *(canary_t *)(stk->data - 1) != CanaryBuf ||
        *(canary_t *)(stk->data + stk->capacity * sizeof(elem_t)) != CanaryBuf) {

                                           ans = ans | incorrect_canary;
    }
    #endif

    #ifdef HASH_MODE
    if (stk->hash != stack_calculate(stk)) ans = ans | incorrect_hash;

    #endif

    return ans;
}


void print_errors(const struct Stack *stk, int err) {

    if (err & stack_null)        fprintf(LOG_FILE, "ERROR! Pointer to stk is NULL\n\n");
    if (err & data_null)         fprintf(LOG_FILE, "ERROR! Pointer to stk.data is NULL\n\n");
    if (err & negative_size)     fprintf(LOG_FILE, "ERROR! size < 0\n\n");
    if (err & negative_capacity) fprintf(LOG_FILE, "ERROR! capacity < 0\n\n");
    if (err & small_capacity)    fprintf(LOG_FILE, "ERROR! size > capacity \n\n");
    #ifdef CANARY_MODE
    if (err & incorrect_canary)  fprintf(LOG_FILE, "ERROR! Value of canary has been changed\n\n");
    #endif
    #ifdef HASH_MODE
    if (err & incorrect_hash)    fprintf(LOG_FILE, "ERROR! Value of hash has been changed\n\n");
    #endif

}


void stack_dump_err(const struct Stack *stk, const char *file, int line, const char *function, FILE* fp) {

    fprintf(fp, "stack_dump from file: %s line %d function: %s stack: %s\n\n",
                                             file, line, function, stk->name);

    #ifdef CANARY_MODE
    fprintf(fp, "struct beginning canary = %X\n", stk->canary1);
    fprintf(fp, "struct end canary = %X\n\n", stk->canary2);
    fprintf(fp, "data beginning canary = %X\n", *(canary_t *)(stk->data - 1));
    fprintf(fp, "data end canary = %X\n\n\n", *(canary_t *)(stk->data + stk->capacity * sizeof(elem_t)));
    #endif

    #ifdef HASH_MODE
    fprintf(fp, "hash = " ELEMF "\n\n", stk->hash);
    #endif

    fprintf(fp, "size = %d\n", stk->size);
    fprintf(fp, "capacity = %d\n", stk->capacity);

    for (int i = 0; i < stk->size; i++) {
        fprintf(fp, "*[%d] = " ELEMF "\n", i, stk->data[i]);
    }

    for (int i = stk->size; i < stk->capacity; i++) {
        fprintf(fp, "[%d] = POISONED\n", i);
    }

}





