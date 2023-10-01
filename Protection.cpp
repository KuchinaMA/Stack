#include <stdio.h>

#include "Types.h"
#include "Stack.h"
#include "Protection.h"
#include "Logfile.h"

hash_t data_hash_calculate(const struct Stack *stk) {

    hash_t hash = 5381;

    char *data_byte_ptr = (char *) stk->data;
    size_t data_byte_size = (size_t) stk->size * sizeof(elem_t);

    for(size_t i = 1; i <= data_byte_size; ++i) {
        hash = ((hash << 5) + hash) ^ data_byte_ptr[i];
    }

    return hash;
}

hash_t struct_hash_calculate(const struct Stack *stk) {

    hash_t hash = 5381;

    hash = data_hash_calculate(stk);

    hash = ((hash << 5) + hash) ^ (char)stk->size;
    hash = ((hash << 5) + hash) ^ (char)stk->capacity;
    hash = ((hash << 5) + hash) ^ (hash_t)&stk->name;

    return hash;
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

    USE_HASH(if (stk->data_hash != data_hash_calculate(stk)) ans = ans | IncorrectHash;
             if (stk->struct_hash != struct_hash_calculate(stk)) ans = ans | IncorrectHash;)

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

