#ifndef AR_BEZIER3_INCLUDED
#define AR_BEZIER3_INCLUDED

#include "vec2.h"

struct ar_bezier3 {
    vec2 control_points[4];
};

void ar_bezier3_init(struct ar_bezier3 *curve, vec2 a, vec2 b, vec2 c, vec2 d);
vec2 ar_bezier3_at(const struct ar_bezier3 *curve, double t);
void ar_bezier3_points(const struct ar_bezier3 *curve, vec2 *points, int n, double t0, double t1);

#endif
