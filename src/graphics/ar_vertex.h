#ifndef AR_VERTEX_INCLUDED
#define AR_VERTEX_INCLUDED

#include <stdint.h>

struct ar_vertex {
    float x;
    float y;
    float u;
    float v;

    float color[4];

    float cx_lower;
    float cy_lower;
    float cx_upper;
    float cy_upper;

    float r_lower;
    float r_upper;

    float type_lower;
    float type_upper;
};

struct ar_vertex ar_vert(float x, float y, float u, float v, uint32_t color);


void ar_make_rect(
    struct ar_vertex *vertices,
    float x0, float y0, float x1, float y1,
    float u0, float v0, float u1, float v1,
    uint32_t color
);

#endif
