#ifndef AR_ARRAY_INCLUDED
#define AR_ARRAY_INCLUDED

#include <stddef.h>
#include <stdlib.h>

#define AR_ARRAY_DECLARATION(array_name, array_value) \
\
struct array_name {\
    array_value *values;\
    size_t n, capacity;\
};\
\
void array_name##_init(struct array_name *a, size_t n, size_t capacity);\
void array_name##_free(struct array_name *a);\
void array_name##_reserve(struct array_name *a, size_t capacity);\
void array_name##_push(struct array_name *a, array_value value);\
array_value array_name##_pop(struct array_name *a);

#define AR_ARRAY_IMPLEMENTATION(array_name, array_value) \
\
void array_name##_init(struct array_name *a, size_t n, size_t capacity){\
    a->values = malloc(sizeof(*a->values)*capacity);\
    a->n = n;\
    a->capacity = capacity;\
}\
\
void array_name##_free(struct array_name *a){\
    free(a->values);\
}\
\
void array_name##_reserve(struct array_name *a, size_t capacity){\
    if (a->capacity >= capacity) return;\
\
    /* create bigger array with values of old array */\
    struct array_name temp[1];\
    array_name##_init(temp, a->n, capacity);\
    memcpy(temp->values, a->values, a->n*sizeof(*a->values));\
\
    /* replace old array with bigger array */\
    array_name##_free(a);\
    *a = *temp;\
}\
\
void array_name##_push(struct array_name *a, array_value value){\
    /* make sure there is space to push to */\
    if (a->n >= a->capacity){\
        array_name##_reserve(a, a->n + a->n/2 + 1);\
    }\
\
    assert(a->n < a->capacity);\
    a->values[a->n++] = value;\
}\
\
array_value array_name##_pop(struct array_name *a){\
    assert(a->n > 0);\
    return a->values[--a->n];\
}

#endif
