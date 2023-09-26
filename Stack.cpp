#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#define CANARY_MODE
#define HASH_MODE

#define STACK_VERIFY(stk, err) {if (stack_verify(stk, err) > 0) { \
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

    //stk->data = (elem_t *)calloc(capacity + 2, sizeof(elem_t));
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

        //stk->data[0] = CanaryBuf; // канарейка на начало буфера
        //stk->data[capacity + 1] = CanaryBuf; //канарейка на конец буфера

        #ifdef HASH_MODE
        stk->hash = stack_calculate(stk);
        #endif
    }

    assert(stk->data != NULL);

}


int stack_dtor(Stack *stk) {

    Errors err = {};

    STACK_VERIFY(stk, &err);

    free((canary_t *)stk->data - 1);
    stk->size = PoisonValue;
    stk->capacity = PoisonValue;
    stk->hash = PoisonValue;

    return 0;

}


int stack_push(Stack *stk, elem_t value) {

    Errors err = {};

    STACK_VERIFY(stk, &err);

    if (stk->size == stk->capacity) {
        stack_realloc(stk, Coeff*stk->capacity);
    }

    stk->data[stk->size] = value;
    stk->size ++;
    stk->hash = stack_calculate(stk);

    STACK_VERIFY(stk, &err);

    return 0;
}


elem_t stack_pop(Stack *stk) {

    Errors err = {};

    STACK_VERIFY(stk, &err);

    stk->size --;
    elem_t ans = stk->data[stk->size];    // implicit canary fix
    stk->data[stk->size] = PoisonValue;

    stk->hash = stack_calculate(stk);

    if (stk->size == (stk->capacity)/Coeff) {
        stack_realloc(stk, stk->capacity/Coeff);
    }

    STACK_VERIFY(stk, &err);

    return ans;
}


void stack_realloc(Stack *stk, int newcapacity) {

    Errors err = {};

    STACK_VERIFY(stk, &err);

    //elem_t temp_canary = stk->data[stk->capacity + 1];

    #ifdef CANARY_MODE
    canary_t temp_canary = *(canary_t *)(stk->data + stk->capacity * sizeof(elem_t));
    *(canary_t *)(stk->data + stk->capacity * sizeof(elem_t)) = PoisonValue;

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

    //*(canary_t *)(stk->data + stk->capacity) = temp_canary;

}


void stack_dump(const struct Stack *stk, const char *file, int line, const char *function) {

    printf("stack_dump from file: %s line %d function: %s\n\n", file, line, function);

    //printf("struct beginning canary = %X\n", stk->canary1);
    //printf("struct end canary = %X\n\n", stk->canary2);

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

    //printf("hash = " ELEMF "\n\n", stk->hash);

    //printf("data beginning canary = %X\n", *(canary_t *)(stk->data - ));

    for (int i = 0; i < stk->size; i++) {
        printf("*[%d] = " ELEMF "\n", i, stk->data[i]);
    }

    for (int i = stk->size; i <= stk->capacity; i++) {
        printf("[%d] = POISONED\n", i);
    }

    //printf("data end canary = %X\n\n\n", *(canary_t *)(stk->data + stk->capacity));
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


int stack_verify (const struct Stack *stk, struct Errors *err) {

    int ans = 0;

    if (stk == NULL) {
        err->stack_null = 1;
        ans ++;
    }
    if (stk->data == NULL) {
        err->data_null = 1;
        ans ++;
    }
    if (stk->size < 0) {
        err->negative_size = 1;
        ans ++;
    }
    if (stk->capacity < 0) {
        err->negative_capacity = 1;
        ans ++;
    }
    if (stk->capacity < stk->size) {
        //ans = ans | err->small_capacity
        err->small_capacity = 1;
        ans ++;
    }
    #ifdef CANARY_MODE
    if (stk->canary1 != CanaryStack || stk->canary2 != CanaryStack ||
        *(canary_t *)(stk->data - 1) != CanaryBuf ||
        *(canary_t *)(stk->data + stk->capacity * sizeof(elem_t)) != CanaryBuf) {
        err->incorrect_canary = 1;
        ans ++;
    }
    #endif

    #ifdef HASH_MODE
    if (stk->hash != stack_calculate(stk)) {
        err->incorrect_hash = 1;
        ans ++;
    }
    #endif

    return ans;
}


void print_errors(const struct Stack *stk, const struct Errors *err) {

    if (err->stack_null)        fprintf(LOG_FILE, "ERROR! Pointer to stk is NULL\n\n");
    if (err->data_null)         fprintf(LOG_FILE, "ERROR! Pointer to stk.data is NULL\n\n");
    if (err->negative_size)     fprintf(LOG_FILE, "ERROR! size < 0\n\n");
    if (err->negative_capacity) fprintf(LOG_FILE, "ERROR! capacity < 0\n\n");
    if (err->small_capacity)    fprintf(LOG_FILE, "ERROR! size > capacity \n\n");
    #ifdef CANARY_MODE
    if (err->incorrect_canary)  fprintf(LOG_FILE, "ERROR! Value of canary has been changed\n\n");
    #endif
    #ifdef HASH_MODE
    if (err->incorrect_hash)    fprintf(LOG_FILE, "ERROR! Value of hash has been changed\n\n");
    #endif

    //stack_dump_err((stk), __FILE__, __LINE__, __func__, LOG_FILE);

}


void stack_dump_err(const struct Stack *stk, const char *file, int line, const char *function, FILE* fp) {

    fprintf(fp, "stack_dump from file: %s line %d function: %s\n\n", file, line, function);

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

    //fprintf(fp, "hash = " ELEMF "\n\n", stk->hash);

    //fprintf(fp, "data beginning canary = %X\n", *(canary_t *)(stk->data - 1));
    //fprintf(fp, "data end canary = %X\n\n\n", *(canary_t *)(stk->data + stk->capacity));

    for (int i = 0; i < stk->size; i++) {
        fprintf(fp, "*[%d] = " ELEMF "\n", i, stk->data[i]);
    }

    for (int i = stk->size; i <= stk->capacity; i++) {
        fprintf(fp, "[%d] = POISONED\n", i);
    }

    //fprintf(fp, "data end canary = %X\n\n\n", *(canary_t *)(stk->data + stk->capacity));
}





