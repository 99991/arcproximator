#ifndef MY_BEZIER3_INCLUDED
#define MY_BEZIER3_INCLUDED

#include "draw.h"

struct my_bezier3 {
    vec2 control_points[4];
};

void my_bezier3_init(struct my_bezier3 *curve, vec2 a, vec2 b, vec2 c, vec2 d){
    vec2 *p = curve->control_points;
    p[0] = a;
    p[1] = b;
    p[2] = c;
    p[3] = d;
}

vec2 my_bezier3_at(const struct my_bezier3 *curve, double t){
    const vec2 *p = curve->control_points;
    double s = 1.0 - t;
    double x = s*s*s*p[0].x + 3.0*s*s*t*p[1].x + 3.0*s*t*t*p[2].x + t*t*t*p[3].x;
    double y = s*s*s*p[0].y + 3.0*s*s*t*p[1].y + 3.0*s*t*t*p[2].y + t*t*t*p[3].y;
    return v2(x, y);
}

void my_bezier3_points(const struct my_bezier3 *curve, vec2 *points, int n){
    int i;
    for (i = 0; i < n; i++){
        double t = i*(1.0/(n - 1));
        points[i] = my_bezier3_at(curve, t);
    }
}

void my_bezier3_draw(const struct my_bezier3 *curve){
    int n = 100;
    vec2 points[n];
    my_bezier3_points(curve, points, n);
    draw(points, n, GL_LINE_STRIP);
}

#endif
