#include "ar_vertex.h"
#include "ar_color.h"

struct ar_vertex ar_vert(float x, float y, float u, float v, uint32_t color){
    struct ar_vertex vertex;

    vertex.x = x;
    vertex.y = y;
    vertex.u = u;
    vertex.v = v;
    ar_color_to_float(color, vertex.color);

    return vertex;
}

void ar_make_rect(
    struct ar_vertex *vertices,
    float x0, float y0, float x1, float y1,
    float u0, float v0, float u1, float v1,
    uint32_t color
){
    vertices[0] = ar_vert(x0, y0, u0, v0, color);
    vertices[1] = ar_vert(x1, y0, u1, v0, color);
    vertices[2] = ar_vert(x1, y1, u1, v1, color);

    vertices[3] = ar_vert(x0, y0, u0, v0, color);
    vertices[4] = ar_vert(x1, y1, u1, v1, color);
    vertices[5] = ar_vert(x0, y1, u0, v1, color);
}
