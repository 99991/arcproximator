#ifndef AR_VERTEX_INCLUDED
#define AR_VERTEX_INCLUDED

#include <stdint.h>

struct ar_vertex {
    float x;
    float y;
    float u;
    float v;

    uint32_t color;

    float cx_lower;
    float cy_lower;
    float cx_upper;
    float cy_upper;

    float r_lower;
    float r_upper;

    float alpha_lower;
    float alpha_upper;

    float lower_y;
    float upper_y;
    float pad0;
    float pad1;
};

#endif
