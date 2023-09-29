#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <stdio.h>

//#define ELEMF "%lld"

//typedef long long int elem_t;

#define ELEMF "%lg"
typedef double elem_t;
const elem_t PoisonValue = -1;

typedef double hash_t;
typedef unsigned long long canary_t;
const canary_t PoisonCanaryValue = -1;

#endif // TYPES_H_INCLUDED
