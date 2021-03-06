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

void ar_bezier3_split(const struct ar_bezier3 *curve, double t, struct ar_bezier3 *curves){
    const vec2 *p = curve->control_points;
    vec2 *q = curves[0].control_points;
    vec2 *r = curves[1].control_points;

    vec2 a = p[0];
    vec2 b = p[1];
    vec2 c = p[2];
    vec2 d = p[3];

    vec2 ab = v2lerp(a, b, t);
    vec2 bc = v2lerp(b, c, t);
    vec2 cd = v2lerp(c, d, t);

    vec2 abc = v2lerp(ab, bc, t);
    vec2 bcd = v2lerp(bc, cd, t);

    vec2 abcd = v2lerp(abc, bcd, t);

    q[0] = a;
    q[1] = ab;
    q[2] = abc;
    q[3] = abcd;

    r[0] = abcd;
    r[1] = bcd;
    r[2] = cd;
    r[3] = d;
}

int ar_bezier3_subdivide(struct ar_bezier3 *curve, vec2 *points, int max_n, double max_angle){
    vec2 *p = curve->control_points;
    vec2 v = v2sub(p[1], p[0]);
    int i;
    double t = 0.0;
    double eps = 0.001;
    vec2 a = p[0];
    points[0] = a;
    for (i = 1; i < max_n - 1; i++){
        while (t < 1.0){
            t += eps;

            vec2 b = ar_bezier3_at(curve, t);
            vec2 w = v2sub(b, a);

            if (v2angle2(v, w) > max_angle){
                points[i] = a;
                a = b;
                v = w;
                break;
            }
        }
        if (t >= 1.0){
            break;
        }
    }
    points[i++] = p[3];

    return i;
}
