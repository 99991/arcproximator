#ifndef AR_VERTEX_INCLUDED
#define AR_VERTEX_INCLUDED

#include <stdint.h>

struct ar_vertex {
    float x;
    float y;
    float u;
    float v;
    uint32_t color;
};

#endif
