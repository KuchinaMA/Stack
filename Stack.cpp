#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define STACK_VERIFY(stk, err) {if (stack_verify(stk, err) > 0) { \
            print_errors(stk, err); }\
}


#include "Types.h"
#include "Stack.h"
#include "Logfile.h"

int stack_ctor(Stack *stk, int capacity) {

    assert(stk != NULL);

    stk->canary1 = CanaryStack;
    stk->canary2 = CanaryStack;

    stk->data = (elem_t *)calloc(capacity + 2, sizeof(elem_t));
    if (stk->data == NULL) {
        printf("Sorry! Capacity is too big, there's no enough memory");
    }
    else {
        stk->size = 0;
        stk->capacity = capacity;

        stk->data[0] = CanaryBuf; // канарейка на начало буфера
        stk->data[capacity + 1] = CanaryBuf; //канарейка на конец буфера

        stk->hash = stack_calculate(stk);
    }
    assert(stk->data != NULL);

}


int stack_dtor(Stack *stk) {

    Errors err = {};

    STACK_VERIFY(stk, &err);

    free(stk->data);
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

    stk->data[stk->size + 1] = value;
    stk->size ++;
    stk->hash = stack_calculate(stk);

    STACK_VERIFY(stk, &err);

    return 0;
}


elem_t stack_pop(Stack *stk) {

    Errors err = {};

    STACK_VERIFY(stk, &err);

    stk->size --;
    elem_t ans = stk->data[stk->size + 1];    // implicit canary fix
    stk->data[stk->size + 1] = PoisonValue;

    stk->hash = stack_calculate(stk);

    if (stk->size == (stk->capacity)/Coeff) {
        stack_realloc(stk, stk->capacity/Coeff);
    }

    STACK_VERIFY(stk, &err);

    return ans;
}


void stack_realloc(Stack *stk, int newsize) {

    Errors err = {};

    STACK_VERIFY(stk, &err);

    elem_t temp_canary = stk->data[stk->capacity + 1];
    stk->data[stk->capacity + 1] = PoisonValue;
    elem_t *temp_data = (elem_t *)realloc(stk->data, (newsize + 2)*sizeof(elem_t));

    if (temp_data != NULL) {
        stk->data = temp_data;
        stk->capacity = newsize;
    }

    stk->data[stk->capacity + 1] = temp_canary;

}


void stack_dump(const struct Stack *stk, const char *file, int line, const char *function) {

    printf("stack_dump from file: %s line %d function: %s\n\n", file, line, function);

    printf("struct beginning canary = %X\n", stk->canary1);
    printf("struct end canary = %X\n\n", stk->canary2);

    printf("size = %d\n", stk->size);
    printf("capacity = %d\n", stk->capacity);

    printf("hash = " ELEMF "\n\n", stk->hash);

    printf("data beginning canary = %X\n", stk->data[0]);

    for (int i = 1; i <= stk->size; i++) {
        printf("*[%d] = " ELEMF "\n", i - 1, stk->data[i]);
    }

    for (int i = stk->size + 1; i <= stk->capacity + 1; i++) {
        printf("[%d] = POISONED\n", i - 1);
    }

    printf("data end canary = %X\n\n\n", stk->data[stk->capacity + 1]);
}


hash_t stack_calculate(const struct Stack *stk) {

    hash_t sum = 0;

    for (int i = 1; i <= stk->size; i++) {
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
    if (stk->canary1 != CanaryStack || stk->canary2 != CanaryStack ||
        stk->data[0] != CanaryBuf || stk->data[stk->capacity + 1] != CanaryBuf) {
        err->incorrect_canary = 1;
        ans ++;
    }
    if (stk->hash != stack_calculate(stk)) {
        err->incorrect_hash = 1;
        ans ++;
    }
    return ans;
}


void print_errors(const struct Stack *stk, const struct Errors *err) {

    //FILE* fp = openlog("Stackerrors");

    if (err->stack_null)        fprintf(LOG_FILE, "ERROR! Pointer to stk is NULL\n\n");
    if (err->data_null)         fprintf(LOG_FILE, "ERROR! Pointer to stk.data is NULL\n\n");
    if (err->negative_size)     fprintf(LOG_FILE, "ERROR! size < 0\n\n");
    if (err->negative_capacity) fprintf(LOG_FILE, "ERROR! capacity < 0\n\n");
    if (err->small_capacity)    fprintf(LOG_FILE, "ERROR! size > capacity \n\n");
    if (err->incorrect_canary)  fprintf(LOG_FILE, "ERROR! Value of canary has been changed\n\n");
    if (err->incorrect_hash)    fprintf(LOG_FILE, "ERROR! Value of hash has been changed\n\n");

    stack_dump_err((stk), __FILE__, __LINE__, __func__, LOG_FILE);

    //closelog(fp);
}

//ошибки без лог файла

/*void print_errors(const struct Errors *err) {

    if (err->stack_null)        printf("ERROR! Pointer to stk is NULL\n\n");
    if (err->data_null)         printf("ERROR! Pointer to stk.data is NULL\n\n");
    if (err->negative_size)     printf("ERROR! size < 0\n\n");
    if (err->negative_capacity) printf("ERROR! capacity < 0\n\n");
    if (err->small_capacity)    printf("ERROR! size > capacity \n\n");
    if (err->incorrect_canary)  printf("ERROR! Value of canary has been changed\n\n");
    if (err->incorrect_hash)    printf("ERROR! Value of hash has been changed\n\n");
}*/



void stack_dump_err(const struct Stack *stk, const char *file, int line, const char *function, FILE* fp) {

    fprintf(fp, "stack_dump from file: %s line %d function: %s\n\n", file, line, function);

    fprintf(fp, "struct beginning canary = %X\n", stk->canary1);
    fprintf(fp, "struct end canary = %X\n\n", stk->canary2);

    fprintf(fp, "size = %d\n", stk->size);
    fprintf(fp, "capacity = %d\n", stk->capacity);

    fprintf(fp, "hash = " ELEMF "\n\n", stk->hash);

    fprintf(fp, "data beginning canary = %X\n", stk->data[0]);

    for (int i = 1; i <= stk->size; i++) {
        fprintf(fp, "*[%d] = " ELEMF "\n", i - 1, stk->data[i]);
    }

    for (int i = stk->size + 1; i <= stk->capacity + 1; i++) {
        fprintf(fp, "[%d] = POISONED\n", i - 1);
    }

    fprintf(fp, "data end canary = %X\n\n\n", stk->data[stk->capacity + 1]);
}





