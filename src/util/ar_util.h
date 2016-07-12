#ifndef AR_UTIL_INCLUDED
#define AR_UTIL_INCLUDED

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define AR_PI 3.14159265358979323846264338327950
#define AR_DBL_INF (1.0/0.0)

#define AR_STR(x) #x

#define AR_UNUSED(x) ((void)x)

#define AR_ASSERT_GOOD_NUMBER(x) do { if (x != x || x == AR_DBL_INF || x == -AR_DBL_INF){ printf("ERROR: %s is %f\n", #x, x); /*exit(-1);*/ } }while(0)

#define AR_SWAP(T, a, b) do { T temp = a; a = b; b = temp; } while(0)

#define AR_INLINE inline __attribute__((always_inline))

#endif
