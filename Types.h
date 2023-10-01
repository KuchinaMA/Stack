#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <stdio.h>

//#define ELEMF "%lld"
//typedef long long int elem_t;

#define ELEMF "%lld"
///Type of elements in stack data
typedef long long int elem_t;
///Value that elem_t elements will have after destruction
const elem_t PoisonValue = -1;

///Type of hash value
typedef long long int hash_t;

///Type of canaries
typedef unsigned long long canary_t;
///Value that canaries will have after destruction
const canary_t PoisonCanaryValue = -1;

#endif // TYPES_H_INCLUDED
