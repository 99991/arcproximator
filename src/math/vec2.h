#ifndef VEC2_INCLUDED
#define VEC2_INCLUDED

#include "../util/ar_util.h"

typedef struct {
    double x;
    double y;
} vec2;

vec2   v2         (double x, double y);
vec2   v2add      (vec2 a, vec2 b);
vec2   v2sub      (vec2 a, vec2 b);
vec2   v2smul     (double a, vec2 b);
vec2   v2polar    (double angle, double radius);
vec2   v2left     (vec2 a);
vec2   v2right    (vec2 a);
vec2   v2neg      (vec2 a);
vec2   v2scale    (vec2 a, double s);
vec2   v2normalize(vec2 a);
vec2   v2lerp     (vec2 a, vec2 b, double u);
double v2det      (vec2 a, vec2 b);
double v2dot      (vec2 a, vec2 b);
double v2len2     (vec2 a);
double v2len      (vec2 a);
double v2dist2    (vec2 a, vec2 b);
double v2dist     (vec2 a, vec2 b);
double v2angle    (vec2 a);
int    v2isleft   (vec2 p, vec2 a, vec2 b);
int    v2isright  (vec2 p, vec2 a, vec2 b);
double v2dist_line(vec2 p, vec2 a, vec2 b);

#endif
