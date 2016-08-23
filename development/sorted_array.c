#include <stdio.h>
#include <stdlib.h>

#include "arcproximator/src/util/ar_array.h"
#include "arcproximator/src/util/ar_util.h"

#ifdef _WIN32
#include <windows.h>

double sec(){
    LARGE_INTEGER frequency, t;
    QueryPerformanceCounter(&t);
    QueryPerformanceFrequency(&frequency);
    return t.QuadPart / (double)frequency.QuadPart;
}
#else
#include <time.h>

double sec(){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + 1e-9*t.tv_nsec;
}
#endif

int cmp_int(int a, int b){
    if (a < b) return -1;
    if (a > b) return +1;
    return 0;
}

struct foo {
    int value;
};

int cmp_foo(const void *p, const void *q){
    return cmp_int(((const struct foo*)p)->value, ((const struct foo*)q)->value);
}

int is_less(const struct foo *a, const struct foo *b){
    return a->value < b->value;
}

AR_ARRAY_DECLARATION(ar_int_array, int)
AR_ARRAY_IMPLEMENTATION(ar_int_array, int)

#define AR_BOUNDS_DECLARATION(name, value_type) \
size_t name##_lower_bound(int *a, size_t n, int value);\
size_t name##_upper_bound(int *a, size_t n, int value);

#define AR_BOUNDS_IMPLEMENTATION(name, value_type, is_less) \
\
size_t name##_lower_bound(value_type *a, size_t n, value_type value){\
    size_t i = 0;\
    while (n > 0){\
        size_t half = n/2;\
        size_t j = i + half;\
        if (is_less(&a[j], &value)){\
            i = j + 1;\
            n -= half + 1;\
        }else{\
            n = half;\
        }\
    }\
    return i;\
}\
\
size_t name##_upper_bound(value_type *a, size_t n, value_type value){\
    size_t i = 0;\
    while (n > 0){\
        size_t half = n/2;\
        size_t j = i + half;\
        if (is_less(&value, &a[j])){\
            n = half;\
        }else{\
            i = j + 1;\
            n -= half + 1;\
        }\
    }\
    return i;\
}

AR_BOUNDS_DECLARATION(ar_int, int)

void ar_int_array_insert(struct ar_int_array *a, size_t index, int value){
    assert(index <= a->n);

    if (a->n >= a->capacity){
        ar_int_array_reserve(a, a->n + a->n/2 + 1);
    }

    /* move [index, last] one to the right to make space for value */
    size_t i;
    for (i = a->n; i > index; i--) a->values[i] = a->values[i-1];
    a->values[index] = value;
    a->n++;
}

int int_is_less(const int *a, const int *b){
    return *a < *b;
}

void ar_int_array_insert_sorted(struct ar_int_array *a, int value){
    size_t i = a->n;
    ar_int_array_push(a, value);
    for (; i > 0 && int_is_less(&value, &a->values[i-1]); i--){
        a->values[i] = a->values[i-1];
    }
    a->values[i] = value;
}

int main(){

    struct ar_int_array a[1], b[1];
    ar_int_array_init(a);
    ar_int_array_init(b);

    size_t n = 10;
    for (size_t i = 0; i < n; i++){
        int x = rand() % 100;
        ar_int_array_insert_sorted(a, x);
        ar_int_array_push(b, x);
    }

    qsort(b->values, n, sizeof(*a->values), cmp_foo);

    for (size_t i = 0; i < n; i++){
        assert(a->values[i] == b->values[i]);
    }

    ar_int_array_free(a);
    ar_int_array_free(b);

{

    int a[] = {1, 2, 3, 3, 7, 8, 8, 8, 10};
    size_t n = sizeof(a)/sizeof(*a);

    for (size_t i = 0; i < n; i++){
        int x = a[i];
        size_t j0 = ar_int_lower_bound(a, n, x);
        size_t j1 = ar_int_upper_bound(a, n, x);
        for (size_t j = j0; j < j1; j++){
            assert(a[j] == x);
        }
        assert(a[j0] == x);
        assert(a[j1-1] == x);
        assert(j1 >= n || a[j1] > x);
    }
    assert(0 == ar_int_lower_bound(a, n, -10));
    assert(0 == ar_int_upper_bound(a, n, -10));
    assert(n == ar_int_lower_bound(a, n, 100));
    assert(n == ar_int_upper_bound(a, n, 100));
}

    return 0;
}

AR_BOUNDS_IMPLEMENTATION(ar_int, int, int_is_less)
