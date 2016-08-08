#include <stdio.h>
#include <stdlib.h>

#include "arcproximator/src/util/ar_array.h"
#include "arcproximator/src/util/ar_util.h"

#define AR_MIN_HEAP_DECLARATION(heap_name, array_name, heap_value) \
void heap_name##_bubble_up(struct array_name *a, size_t index);\
void heap_name##_push(struct array_name *a, heap_value value);\
void heap_name##_sink_down(struct array_name *a, size_t index);\
heap_value heap_name##_pop(struct array_name *a);

#define AR_MIN_HEAP_IMPLEMENTATION(heap_name, array_name, heap_value, is_less) \
\
void heap_name##_bubble_up(struct array_name *a, size_t index){\
    heap_value *values = a->values;\
    while (index > 0){\
        size_t parent = (index - 1)/2;\
\
        /* if parent is smaller, heap property is fulfiled and we are done*/\
        if (is_less(&values[parent], &values[index])) return;\
\
        AR_SWAP(heap_value, values[parent], values[index]);\
        index = parent;\
    }\
}\
\
void heap_name##_push(struct array_name *a, heap_value value){\
    size_t index = a->n;\
    array_name##_push(a, value);\
    heap_name##_bubble_up(a, index);\
}\
\
void heap_name##_sink_down(struct array_name *a, size_t index){\
    heap_value *values = a->values;\
    while (1){\
        size_t child1 = index*2 + 1;\
        size_t child2 = index*2 + 2;\
\
        /* find smaller child */\
        size_t smallest = index;\
        if (child1 < a->n && is_less(&values[child1], &values[smallest])) smallest = child1;\
        if (child2 < a->n && is_less(&values[child2], &values[smallest])) smallest = child2;\
\
        /* if no child is smaller, heap property is fulfiled and we are done */\
        if (smallest == index) return;\
\
        AR_SWAP(heap_value, values[index], values[smallest]);\
        index = smallest;\
    }\
}\
\
heap_value heap_name##_pop(struct array_name *a){\
    if (a->n == 1) return array_name##_pop(a);\
\
    /* remember minimum so we can return it at the end */\
    heap_value minimum = a->values[0];\
\
    /* overwrite the minimum with last value (violates heap property) */\
    a->values[0] = array_name##_pop(a);\
\
    /* fix heap property */\
    heap_name##_sink_down(a, 0);\
\
    return minimum;\
}

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

AR_ARRAY_DECLARATION(ar_foo_array, struct foo)
AR_MIN_HEAP_DECLARATION(ar_min_heap_foo, ar_foo_array, struct foo)

int main(){

    size_t i, j, m = 20;
    struct foo *a = malloc(m*sizeof(*a));
    struct foo *b = malloc(m*sizeof(*b));
    struct ar_foo_array h[1];
    ar_foo_array_init_n(h, 0, m);

    for (j = 0; j < 100*1000; j++){
        size_t n = rand() % m;

        assert(h->n == 0);

        for (i = 0; i < n; i++){
            int x = rand() % 10;
            struct foo f;
            f.value = x;
            ar_min_heap_foo_push(h, f);
            a[i] = f;
        }

        for (i = 0; i < n; i++){
            b[i] = ar_min_heap_foo_pop(h);
        }

        qsort(a, n, sizeof(*a), cmp_foo);

        for (i = 0; i < n; i++){
            assert(a[i].value == b[i].value);
        }
    }

    ar_foo_array_free(h);
    free(a);
    free(b);

    return 0;
}

AR_ARRAY_IMPLEMENTATION(ar_foo_array, struct foo)
AR_MIN_HEAP_IMPLEMENTATION(ar_min_heap_foo, ar_foo_array, struct foo, is_less)
