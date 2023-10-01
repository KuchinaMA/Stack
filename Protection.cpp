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

    if (stk == NULL)                                ans = ans | StackNull;

    if (stk->data == NULL)                          ans = ans | DataNull;

    if (stk->size < 0)                              ans = ans | NegativeSize;

    if (stk->capacity < 0)                          ans = ans | NegativeCapacity;

    if (stk->capacity < stk->size)                  ans = ans | SmallCapacity;


    USE_CANARIES(if (stk->canary1 != CanaryStack || stk->canary2 != CanaryStack ||
        *(canary_t *)(stk->data - 1) != CanaryData ||
        *(canary_t *)(stk->data + stk->capacity * sizeof(elem_t)) != CanaryData) {

                                                    ans = ans | IncorrectCanary;
    })

    USE_HASH(if (stk->hash != stack_calculate(stk)) ans = ans | IncorrectHash;)

    return ans;
}


void print_errors(const struct Stack *stk, int err) {

    if (err & StackNull)        fprintf(LOG_FILE, "ERROR! Pointer to stk is NULL\n\n");
    if (err & DataNull)         fprintf(LOG_FILE, "ERROR! Pointer to stk.data is NULL\n\n");
    if (err & NegativeSize)     fprintf(LOG_FILE, "ERROR! size < 0\n\n");
    if (err & NegativeCapacity) fprintf(LOG_FILE, "ERROR! capacity < 0\n\n");
    if (err & SmallCapacity)    fprintf(LOG_FILE, "ERROR! size > capacity \n\n");

    USE_CANARIES(if (err & IncorrectCanary)  fprintf(LOG_FILE,
                                                   "ERROR! Value of canary has been changed\n\n");)

    USE_HASH(if (err & IncorrectHash)    fprintf(LOG_FILE,
                                                   "ERROR! Value of hash has been changed\n\n");)

}

