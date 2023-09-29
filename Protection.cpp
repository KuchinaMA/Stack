#include <stdio.h>

#include "Types.h"
#include "Stack.h"
#include "Protection.h"
#include "Logfile.h"

hash_t stack_calculate(const struct Stack *stk) {

    hash_t sum = 0;

    for (int i = 1; i <= stk->size; i++) {  //because of canary
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
