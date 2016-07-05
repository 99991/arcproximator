#ifndef VEC2_INCLUDED
#define VEC2_INCLUDED

#include "util.h"

typedef struct {
    double x;
    double y;
} vec2;

INLINE vec2 v2(double x, double y){
    vec2 v;

    v.x = x;
    v.y = y;

    return v;
}

INLINE vec2 v2add(vec2 a, vec2 b){
    return v2(a.x + b.x, a.y + b.y);
}

INLINE vec2 v2sub(vec2 a, vec2 b){
    return v2(a.x - b.x, a.y - b.y);
}

INLINE vec2 v2smul(double a, vec2 b){
    return v2(a * b.x, a * b.y);
}

INLINE double v2det(vec2 a, vec2 b){
    return a.x * b.y - a.y * b.x;
}

INLINE double v2dot(vec2 a, vec2 b){
    return a.x * b.x + a.y * b.y;
}

INLINE double v2len2(vec2 a){
    return v2dot(a, a);
}

INLINE double v2len(vec2 a){
    return sqrt(v2len2(a));
}

INLINE double v2dist2(vec2 a, vec2 b){
    return v2len2(v2sub(a, b));
}

INLINE double v2dist(vec2 a, vec2 b){
    return v2len(v2sub(a, b));
}

INLINE vec2 v2polar(double angle, double radius){
    return v2(radius*cos(angle), radius*sin(angle));
}

INLINE double v2angle(vec2 a){
    return atan2(a.y, a.x);
}

INLINE int v2isleft(vec2 p, vec2 a, vec2 b){
    return v2det(v2sub(b, a), v2sub(p, a)) > 0.0;
}

INLINE int v2isright(vec2 p, vec2 a, vec2 b){
    return v2det(v2sub(b, a), v2sub(p, a)) < 0.0;
}

INLINE vec2 v2left(vec2 a){
    return v2(-a.y, a.x);
}

INLINE vec2 v2right(vec2 a){
    return v2(a.y, -a.x);
}

INLINE vec2 v2neg(vec2 a){
    return v2(-a.x, -a.y);
}

INLINE vec2 v2scale(vec2 a, double s){
    return v2smul(s/v2len(a), a);
}

INLINE vec2 v2normalize(vec2 a){
    return v2scale(a, 1.0);
}

#endif
