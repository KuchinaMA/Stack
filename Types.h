#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <stdio.h>

#define ELEMF "%lld"

typedef long long int elem_t;
const elem_t PoisonValue = -1;

typedef long long int hash_t;

typedef unsigned long long int canary_t;

#endif // TYPES_H_INCLUDED
