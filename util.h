#ifndef UTIL_INCLUDED
#define UTIL_INCLUDED

#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifndef PI
#define PI 3.14159265358979323846264338327950
#endif

#ifndef DBL_INF
#define DBL_INF (1.0/0.0)
#endif

#define UNUSED(x) ((void)x)

#define ASSERT_NOT_NAN(x) do { if (x != x){ printf("ERROR: %s is NaN\n", #x); /*exit(-1);*/ } }while(0)

#define MY_SWAP(T, a, b) do { T temp = a; a = b; b = temp; } while(0)

#define INLINE inline __attribute__((always_inline))

INLINE double fmin(double a, double b){
    return a < b ? a : b;
}

INLINE double fmax(double a, double b){
    return a > b ? a : b;
}

INLINE double frand(){
    return rand() / (double)RAND_MAX;
}

#endif
