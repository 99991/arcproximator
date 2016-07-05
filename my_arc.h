#ifndef MY_ARC_INCLUDED
#define MY_ARC_INCLUDED

#include "draw.h"

struct my_arc {
    vec2 center, start, end;
    double radius;
    int clockwise;
};

void my_arc_init(
    struct my_arc *arc,
    vec2 center,
    double radius,
    vec2 start,
    vec2 end,
    int clockwise
){
    arc->center = center;
    arc->start = start;
    arc->end = end;
    arc->radius = radius;
    arc->clockwise = clockwise;
}

void my_arc_points(const struct my_arc *arc, vec2 *points, int n){
    vec2 center = arc->center;
    vec2 start = arc->start;
    vec2 end = arc->end;
    double radius = arc->radius;

    double start_angle = v2angle(v2sub(start, center));
    double end_angle = v2angle(v2sub(end, center));

    if (arc->clockwise){
        MY_SWAP(double, start_angle, end_angle);
    }

    if (end_angle < start_angle) end_angle += 2.0*PI;

    int i;
    for (i = 0; i < n; i++){
        double angle = start_angle + (end_angle - start_angle)/(n - 1) * i;
        points[i] = v2add(center, v2polar(angle, radius));
    }
}

void my_arc_draw(const struct my_arc *arc){
    int n = 100;
    vec2 points[n];
    my_arc_points(arc, points, n);
    draw(points, n, GL_LINE_STRIP);
    /*
    draw_line(arc->center, arc->start);
    draw_line(arc->center, arc->end);
    */
}

int my_ccw_arc_encloses(const struct my_arc *arc, vec2 p){
    int left_start = v2isleft(p, arc->center, arc->start);
    int right_end  = v2isright(p, arc->center, arc->end);

    if (v2isleft(arc->end, arc->center, arc->start)){
        return left_start && right_end;
    }else{
        return left_start || right_end;
    }
}

int my_arc_encloses(const struct my_arc *arc, vec2 p){
    return my_ccw_arc_encloses(arc, p) ^ arc->clockwise;
}

vec2 my_arc_clamp(const struct my_arc *arc, vec2 p){
    if (my_arc_encloses(arc, p)){
        return v2add(arc->center, v2scale(v2sub(p, arc->center), arc->radius));
    }

    return v2dist2(p, arc->start) < v2dist2(p, arc->end) ? arc->start : arc->end;
}

#endif
