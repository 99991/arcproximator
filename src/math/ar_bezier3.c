#include "ar_bezier3.h"

void ar_bezier3_init(struct ar_bezier3 *curve, vec2 a, vec2 b, vec2 c, vec2 d){
    vec2 *p = curve->control_points;
    p[0] = a;
    p[1] = b;
    p[2] = c;
    p[3] = d;
}

vec2 ar_bezier3_at(const struct ar_bezier3 *curve, double t){
    const vec2 *p = curve->control_points;
    double s = 1.0 - t;
    double x = s*s*s*p[0].x + 3.0*s*s*t*p[1].x + 3.0*s*t*t*p[2].x + t*t*t*p[3].x;
    double y = s*s*s*p[0].y + 3.0*s*s*t*p[1].y + 3.0*s*t*t*p[2].y + t*t*t*p[3].y;
    return v2(x, y);
}

void ar_bezier3_points(const struct ar_bezier3 *curve, vec2 *points, int n, double t0, double t1){
    int i;
    for (i = 0; i < n; i++){
        double u = 1.0/(n-1) * i;
        double t = t0 + (t1 - t0)*u;
        points[i] = ar_bezier3_at(curve, t);
    }
}
