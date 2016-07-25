#include "vec2.h"

#include <math.h>

vec2 v2(double x, double y){
    vec2 v;

    v.x = x;
    v.y = y;

    return v;
}

vec2 v2add(vec2 a, vec2 b){
    return v2(a.x + b.x, a.y + b.y);
}

vec2 v2sub(vec2 a, vec2 b){
    return v2(a.x - b.x, a.y - b.y);
}

vec2 v2smul(double a, vec2 b){
    return v2(a * b.x, a * b.y);
}

double v2det(vec2 a, vec2 b){
    return a.x * b.y - a.y * b.x;
}

double v2dot(vec2 a, vec2 b){
    return a.x * b.x + a.y * b.y;
}

double v2len2(vec2 a){
    return v2dot(a, a);
}

double v2len(vec2 a){
    return sqrt(v2len2(a));
}

double v2dist2(vec2 a, vec2 b){
    return v2len2(v2sub(a, b));
}

double v2dist(vec2 a, vec2 b){
    return v2len(v2sub(a, b));
}

vec2 v2polar(double angle, double radius){
    return v2(radius*cos(angle), radius*sin(angle));
}

double v2angle(vec2 a){
    return atan2(a.y, a.x);
}

int v2isleft(vec2 p, vec2 a, vec2 b){
    return v2det(v2sub(b, a), v2sub(p, a)) > 0.0;
}

int v2isright(vec2 p, vec2 a, vec2 b){
    return v2det(v2sub(b, a), v2sub(p, a)) < 0.0;
}

vec2 v2left(vec2 a){
    return v2(-a.y, a.x);
}

vec2 v2right(vec2 a){
    return v2(a.y, -a.x);
}

vec2 v2neg(vec2 a){
    return v2(-a.x, -a.y);
}

vec2 v2scale(vec2 a, double s){
    return v2smul(s/v2len(a), a);
}

vec2 v2normalize(vec2 a){
    return v2scale(a, 1.0);
}

vec2 v2lerp(vec2 a, vec2 b, double u){
    return v2add(v2smul(1.0 - u, a), v2smul(u, b));
}

double v2dist_line(vec2 p, vec2 a, vec2 b){
    vec2 ba = v2sub(b, a);
    double u = v2dot(v2sub(p, a), ba)/v2dot(ba, ba);
    vec2 q = v2lerp(a, b, u);
    return v2dist(p, q);
}
