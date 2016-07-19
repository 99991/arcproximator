#ifndef AR_ARC_INCLUDED
#define AR_ARC_INCLUDED

#include "vec2.h"

enum ar_arc_type {
    /* counterclockwise arc from start to end */
    AR_ARC_COUNTERCLOCKWISE,
    /* clockwise arc from star to end */
    AR_ARC_CLOCKWISE,
    /* arc is actually line from start to end */
    AR_ARC_LINE,
};

struct ar_arc {
    vec2 center, start, end;
    double radius;
    enum ar_arc_type arc_type;
};

void ar_arc_init(
    struct ar_arc *arc,
    vec2 center,
    double radius,
    vec2 start,
    vec2 end,
    enum ar_arc_type arc_type
);

void ar_arc_points(const struct ar_arc *arc, vec2 *points, int n, double t0, double t1);
int ar_ccw_arc_encloses(const struct ar_arc *arc, vec2 p);
int ar_arc_encloses(const struct ar_arc *arc, vec2 p);
vec2 ar_arc_clamp(const struct ar_arc *arc, vec2 p);

#endif
