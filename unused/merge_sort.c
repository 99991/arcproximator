#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct foo {
    int value;
};

int is_less(const struct foo *a, const struct foo *b){
    return a->value < b->value;
}

/* Merge [a0, a1) and [b0, b1) into [a0, b1)where a1 == b0.*/
void ar_merge(struct foo *a0, struct foo *a1, struct foo *b1, struct foo *temp){
    struct foo *b0 = a1;
    /* skip sorted part of [a0, a1) */
    while (a0 < a1 && is_less(a0, b0)) a0++;

    struct foo *temp_ptr = temp;
    struct foo *dst = a0;
    /* merge [a0, a1) and [b0, b1) until one sequence is exhausted */
    while (a0 < a1 && b0 < b1) *temp_ptr++ = is_less(a0, b0) ? *a0++ : *b0++;

    /* merge rest of [a0, a1) */
    while (a0 < a1) *temp_ptr++ = *a0++;

    /* copy back */
    size_t n = temp_ptr - temp;
    memcpy(dst, temp, n*sizeof(*dst));
}

void ar_merge_sort(struct foo *values, struct foo *temp, size_t n){
    if (n <= 1) return;

    size_t half = n/2;
    ar_merge_sort(values, temp, half);
    ar_merge_sort(values + half, temp, n - half);
    ar_merge(values, values + half, values + n, temp);
}

int cmp_int(int a, int b){
    if (a < b) return -1;
    if (a > b) return +1;
    return 0;
}

int cmp_foo(const void *p, const void *q){
    return cmp_int(((const struct foo*)p)->value, ((const struct foo*)q)->value);
}

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

int main(){

    size_t n = 1024*1024;
    for (size_t k = 0; k < 10; k++){
        struct foo *a = malloc(n*sizeof(*a));
        struct foo *b = malloc(n*sizeof(*a));
        struct foo *temp = malloc(n*sizeof(*a));

        for (size_t i = 0; i < n; i++){
            struct foo x;
            x.value = rand();
            a[i] = x;
            b[i] = x;
        }

        double t0 = sec();
        ar_merge_sort(a, temp, n);
        double t1 = sec();
        qsort(b, n, sizeof(*b), cmp_foo);
        double t2 = sec();

        for (size_t i = 0; i < n; i++){
            assert(a[i].value == b[i].value);
        }

        free(a);
        free(b);
        free(temp);

        printf("%f %f, %f times faster\n", t1 - t0, t2 - t1, (t2 - t1)/(t1 - t0));
    }

    return 0;
}
