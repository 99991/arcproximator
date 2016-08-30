#ifndef AR_VERTEX_INCLUDED
#define AR_VERTEX_INCLUDED

#include <stdint.h>

struct ar_vertex {
    float x;
    float y;
    float u;
    float v;
    uint32_t color;

    float x_lower;
    float y_lower;
    float x_upper;
    float y_upper;

    float r_lower;
    float r_upper;

    float alpha_lower;
    float alpha_upper;
};

#endif
