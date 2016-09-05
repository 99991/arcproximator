#include "ar_arc.h"

void ar_arc_init(
    struct ar_arc *arc,
    vec2 center,
    double radius,
    vec2 start,
    vec2 end,
    enum ar_arc_type arc_type
){
    arc->center = center;
    arc->start = start;
    arc->end = end;
    arc->radius = radius;
    arc->arc_type = arc_type;
}

void ar_arc_points(const struct ar_arc *arc, vec2 *points, int n, double t0, double t1){
    int i;
    vec2 start = arc->start;
    vec2 end = arc->end;
    vec2 center = arc->center;
    double radius = arc->radius;

    double start_angle = v2angle(v2sub(start, center));
    double end_angle   = v2angle(v2sub(end, center));

    switch (arc->arc_type){
        case AR_ARC_COUNTERCLOCKWISE:
        break;

        case AR_ARC_CLOCKWISE:
            AR_SWAP(double, start_angle, end_angle);
            AR_SWAP(double, t0, t1);
        break;

        case AR_ARC_LINE:
            for (i = 0; i < n; i++){
                double u = 1.0/(n - 1) * i;
                points[i] = v2lerp(start, end, u);
            }
        return;
    }

    if (end_angle < start_angle) end_angle += 2.0*AR_PI;

    for (i = 0; i < n; i++){
        double u = 1.0/(n - 1) * i;
        double t = t0 + (t1 - t0)*u;
        double angle = start_angle + (end_angle - start_angle)*t;

        points[i] = v2add(center, v2polar(angle, radius));
    }
}

int ar_arc_is_over_180(const struct ar_arc *arc){
    return v2isright(arc->end, arc->center, arc->start) ^ arc->arc_type;
}

int ar_ccw_arc_encloses(const struct ar_arc *arc, vec2 p){
    int left_start = v2isleft(p, arc->center, arc->start);
    int right_end  = v2isright(p, arc->center, arc->end);

    if (v2isleft(arc->end, arc->center, arc->start)){
        return left_start && right_end;
    }else{
        return left_start || right_end;
    }
}

int ar_arc_encloses(const struct ar_arc *arc, vec2 p){
    return ar_ccw_arc_encloses(arc, p) ^ arc->arc_type;
}

vec2 ar_arc_clamp(const struct ar_arc *arc, vec2 p){
    if (ar_arc_encloses(arc, p)){
        return v2add(arc->center, v2scale(v2sub(p, arc->center), arc->radius));
    }

    return v2dist2(p, arc->start) < v2dist2(p, arc->end) ? arc->start : arc->end;
}

void ar_arc_reverse(struct ar_arc *arc){
    AR_SWAP(vec2, arc->start, arc->end);
    arc->arc_type = !arc->arc_type;
}
