#include <stdio.h>

#include "Types.h"
#include "Stack.h"
#include "Protection.h"
#include "Logfile.h"

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

    if (stk == NULL)                                ans = ans | stack_null;

    if (stk->data == NULL)                          ans = ans | data_null;

    if (stk->size < 0)                              ans = ans | negative_size;

    if (stk->capacity < 0)                          ans = ans | negative_capacity;

    if (stk->capacity < stk->size)                  ans = ans | small_capacity;


    USE_CANARIES(if (stk->canary1 != CanaryStack || stk->canary2 != CanaryStack ||
        *(canary_t *)(stk->data - 1) != CanaryData ||
        *(canary_t *)(stk->data + stk->capacity * sizeof(elem_t)) != CanaryData) {

                                                    ans = ans | incorrect_canary;
    })

    USE_HASH(if (stk->hash != stack_calculate(stk)) ans = ans | incorrect_hash;)

    return ans;
}


void print_errors(const struct Stack *stk, int err) {

    if (err & stack_null)        fprintf(LOG_FILE, "ERROR! Pointer to stk is NULL\n\n");
    if (err & data_null)         fprintf(LOG_FILE, "ERROR! Pointer to stk.data is NULL\n\n");
    if (err & negative_size)     fprintf(LOG_FILE, "ERROR! size < 0\n\n");
    if (err & negative_capacity) fprintf(LOG_FILE, "ERROR! capacity < 0\n\n");
    if (err & small_capacity)    fprintf(LOG_FILE, "ERROR! size > capacity \n\n");

    USE_CANARIES(if (err & incorrect_canary)  fprintf(LOG_FILE,
                                                   "ERROR! Value of canary has been changed\n\n");)

    USE_HASH(if (err & incorrect_hash)    fprintf(LOG_FILE,
                                                   "ERROR! Value of hash has been changed\n\n");)

}

