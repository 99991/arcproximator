#include "math/ar_bezier3.h"
#include "math/ar_arc.h"
#include "graphics/ar_shader.h"
#include "graphics/ar_draw.h"
#include "graphics/ar_texture.h"
#include "graphics/ar_color.h"
#include "math/vec2.h"
#include "math/mat23.h"
#include "math/mat4.h"
#include "math/ar_polynomial.h"

#include <assert.h>
#include <stdio.h>

#include <GL/freeglut.h>

struct ar_window {
    int width;
    int height;
    int is_mouse_button_down[256];
    int is_key_down[256];
    vec2 mouse_pos;
    vec2 mouse_pos_old;
    mat23 world_to_screen;
};

struct ar_window window;
struct ar_shader arc_shader[1];
struct ar_texture texture[1];
GLint apos, atex, acol, umvp, utex0;
vec2 control_points[4];

vec2 screen_to_world(vec2 screen_pos){
    return m23mulv2(m23inv(window.world_to_screen), screen_pos);
}

struct ar_vertex ar_vert(float x, float y, float u, float v, uint32_t color){
    struct ar_vertex vertex;

    vertex.x = x;
    vertex.y = y;
    vertex.u = u;
    vertex.v = v;
    vertex.color = color;

    return vertex;
}

void ar_make_rect(
    struct ar_vertex *vertices,
    float x0, float y0, float x1, float y1,
    float u0, float v0, float u1, float v1,
    uint32_t color
){
    vertices[0] = ar_vert(x0, y0, u0, v0, color);
    vertices[1] = ar_vert(x1, y0, u1, v0, color);
    vertices[2] = ar_vert(x1, y1, u1, v1, color);

    vertices[3] = ar_vert(x0, y0, u0, v0, color);
    vertices[4] = ar_vert(x1, y1, u1, v1, color);
    vertices[5] = ar_vert(x0, y1, u0, v1, color);
}

void upload_model_view_projection(mat4 mvp){
    /* upload as double not available everywhere */
    float data[16];
    int i, j;
    for (i = 0; i < 4; i++){
        for (j = 0; j < 4; j++){
            /* column major matrix */
            data[i + j*4] = m4at(mvp, i, j);
        }
    }
    ar_glUniformMatrix4fv(umvp, 1, 0, data);
}

void ar_draw_points(const vec2 *points, int n, uint32_t color, GLenum mode){
    struct ar_vertex *vertices = (struct ar_vertex*)malloc(sizeof(*vertices)*n);
    int i;
    for (i = 0; i < n; i++){
        vec2 p = points[i];
        vertices[i] = ar_vert(p.x, p.y, 0.0f, 0.0f, color);
    }
    ar_draw(vertices, n, mode, apos, atex, acol);
    free(vertices);
}

void ar_draw_line(vec2 a, vec2 b, uint32_t color){
    struct ar_vertex vertices[2];
    vertices[0] = ar_vert(a.x, a.y, 0.0f, 0.0f, color);
    vertices[1] = ar_vert(b.x, b.y, 0.0f, 0.0f, color);
    ar_draw(vertices, 2, GL_LINES, apos, atex, acol);
}

void ar_bezier3_draw(const struct ar_bezier3 *curve, uint32_t color){
    int n = 100;
    vec2 *points = (vec2*)malloc(sizeof*(points)*n);
    ar_bezier3_points(curve, points, n, 0.0, 1.0);
    ar_draw_points(points, n, color, GL_LINE_STRIP);
    free(points);
}

void ar_circle_points(vec2 *points, int n, vec2 center, double radius){
    double delta_angle = 2.0*AR_PI/n;
    int i;
    for (i = 0; i < n; i++){
        double angle = i*delta_angle;
        points[i] = v2add(center, v2polar(angle, radius));
    }
}

void ar_draw_circle(vec2 center, double radius, uint32_t color){
    int n = 100;
    vec2 *points = (vec2*)malloc(sizeof*(points)*n);
    ar_circle_points(points, n, center, radius);
    ar_draw_points(points, n, color, GL_LINE_LOOP);
    free(points);
}

void ar_draw_arc(struct ar_arc *arc, uint32_t color){
    int n = 100;
    vec2 *points = (vec2*)malloc(n*sizeof(*points));
    ar_arc_points(arc, points, n, 0.0, 1.0);
    ar_draw_points(points, n, color, GL_LINE_STRIP);
}

void ar_draw_arrow(vec2 a, vec2 b, double r, uint32_t color){
    vec2 d = v2scale(v2sub(b, a), r);

    vec2 points[3];
    points[0] = a;
    points[1] = b;
    ar_draw_points(points, 2, color, GL_LINES);

    points[0] = b;
    points[1] = v2sub(b, v2add(v2smul(2.0, d), v2left(d)));
    points[2] = v2sub(b, v2sub(v2smul(2.0, d), v2left(d)));

    ar_draw_points(points, 3, color, GL_TRIANGLES);
}

void arc_from_points_and_normal(struct ar_arc *arc, vec2 start, vec2 end, vec2 start_normal, int clockwise){
    /* Create arc from normal, starting and end point. */
    /* start + radius*start_normal = center */
    vec2 v = v2sub(end, start);

    double d = v2dot(v, start_normal);

    double radius = v2dot(v, v)*0.5/d;
    AR_ASSERT_GOOD_NUMBER(radius);

    vec2 center = v2add(start, v2smul(radius, start_normal));

    ar_arc_init(arc, center, radius, start, end, clockwise);
}
#if 0
int line_intersect(vec2 a, vec2 b, vec2 c, vec2 d, double *u, double *v){
    /*
    # points along line should be same
    a + u*ba = c + v*dc

    # solve for u
        # cross both sides with dc
            (a + u*ba) x dc = (c + v*dc) x dc
        # dc x dc = 0
            u (ba x dc) = (c - a) x dc
        # divide by (ba x dc)
            u = (c - a) x dc / (ba x dc)

    # solve for v
        # cross both sides with ba
            (a + u*ba) x ba = (c + v*dc) x ba
        # ba x ba = 0
            (a - c) x ba = v (dc x ba)
        # divide by -(dc x ba) = (ba x dc)
            v = (c - a) x ba / (ba x dc)
    */
    vec2 ba = v2sub(b, a);
    vec2 dc = v2sub(d, c);
    vec2 ca = v2sub(c, a);

    double det = v2det(ba, dc);

    /* TODO check if det is almost zero */

    double inv_det = 1.0/det;

    *u = v2det(ca, dc) * inv_det;
    *v = v2det(ca, ba) * inv_det;

    return 1;
}

int line_segment_intersect(vec2 a, vec2 b, vec2 c, vec2 d, vec2 *intersection){
    double u, v;
    if (!line_intersect(a, b, c, d, &u, &v)) return 0;
    if (u >= 0.0 && u <= 1.0 && v >= 0.0 && v <= 1.0){
        *intersection = v2lerp(a, b, u);
        return 1;
    }
    return 0;
}
#endif

int show_bezier           = 0;
int show_solution_circle  = 0;
int show_max_dist         = 0;
int show_control_points   = 0;
int show_biarc            = 0;

void menu_callback(int option){
    switch (option){
        case 0: show_bezier           = !show_bezier;           break;
        case 1: show_solution_circle  = !show_solution_circle;  break;
        case 3: show_max_dist         = !show_max_dist;         break;
        case 4: show_control_points   = !show_control_points;   break;
        case 5: show_biarc            = !show_biarc;            break;
        default: break;
    }
}

int ar_bezier3_circle_intersection(
    vec2 a, vec2 b, vec2 c, vec2 d,
    vec2 center,
    double radius,
    double tolerance,
    double *t
){
    double ax = a.x;
    double ay = a.y;
    double bx = b.x;
    double by = b.y;
    double cx = c.x;
    double cy = c.y;
    double dx = d.x;
    double dy = d.y;
    double x = center.x;
    double y = center.y;
    double r = radius;

    AR_ASSERT_GOOD_NUMBER(ax);
    AR_ASSERT_GOOD_NUMBER(bx);
    AR_ASSERT_GOOD_NUMBER(cx);
    AR_ASSERT_GOOD_NUMBER(dx);
    AR_ASSERT_GOOD_NUMBER(x);
    AR_ASSERT_GOOD_NUMBER(ay);
    AR_ASSERT_GOOD_NUMBER(by);
    AR_ASSERT_GOOD_NUMBER(cy);
    AR_ASSERT_GOOD_NUMBER(dy);
    AR_ASSERT_GOOD_NUMBER(y);

    double coefficients[7];
    coefficients[0] = (ax*ax) - 6*ax*bx + 6*ax*cx - 2*ax*dx + (ay*ay) - 6*ay*by + 6*ay*cy - 2*ay*dy + 9*(bx*bx) - 18*bx*cx + 6*bx*dx + 9*(by*by) - 18*by*cy + 6*by*dy + 9*(cx*cx) - 6*cx*dx + 9*(cy*cy) - 6*cy*dy + (dx*dx) + (dy*dy);
    coefficients[1] = -6*(ax*ax) + 30*ax*bx - 24*ax*cx + 6*ax*dx - 6*(ay*ay) + 30*ay*by - 24*ay*cy + 6*ay*dy - 36*(bx*bx) + 54*bx*cx - 12*bx*dx - 36*(by*by) + 54*by*cy - 12*by*dy - 18*(cx*cx) + 6*cx*dx - 18*(cy*cy) + 6*cy*dy;
    coefficients[2] = 15*(ax*ax) - 60*ax*bx + 36*ax*cx - 6*ax*dx + 15*(ay*ay) - 60*ay*by + 36*ay*cy - 6*ay*dy + 54*(bx*bx) - 54*bx*cx + 6*bx*dx + 54*(by*by) - 54*by*cy + 6*by*dy + 9*(cx*cx) + 9*(cy*cy);
    coefficients[3] = -20*(ax*ax) + 60*ax*bx - 24*ax*cx + 2*ax*dx + 2*ax*x - 20*(ay*ay) + 60*ay*by - 24*ay*cy + 2*ay*dy + 2*ay*y - 36*(bx*bx) + 18*bx*cx - 6*bx*x - 36*(by*by) + 18*by*cy - 6*by*y + 6*cx*x + 6*cy*y - 2*dx*x - 2*dy*y;
    coefficients[4] = 15*(ax*ax) - 30*ax*bx + 6*ax*cx - 6*ax*x + 15*(ay*ay) - 30*ay*by + 6*ay*cy - 6*ay*y + 9*(bx*bx) + 12*bx*x + 9*(by*by) + 12*by*y - 6*cx*x - 6*cy*y;
    coefficients[5] = -6*(ax*ax) + 6*ax*bx + 6*ax*x - 6*(ay*ay) + 6*ay*by + 6*ay*y - 6*bx*x - 6*by*y;
    coefficients[6] = (ax*ax) - 2*ax*x + (ay*ay) - 2*ay*y - (r*r) + (x*x) + (y*y);

#if 0
    /* Normalize coefficients for more predictable tolerance. */
    /* An error of eps*x^7 with x in [0, 1] should be barely noticeable. */
    double eps = 1e-10;
    if (coefficients[0] < -eps || eps < coefficients[0]){
        int i;
        for (i = 1; i < 7; i++){
            coefficients[i] *= 1.0/coefficients[0];
        }
        coefficients[0] = 1.0;
    }
#endif

    double root;
    int error = ar_polynomial_root(coefficients, 7, 0.0, 1.0, tolerance, &root);
    if (error){
        puts("ERROR with ar_polynomial_root:");
        if (error == AR_POLYNOMIAL_ERROR_BISECTION_INTERVAL_NOT_FOUND){
            puts("AR_POLYNOMIAL_ERROR_BISECTION_INTERVAL_NOT_FOUND");

        }else if (error == AR_POLYNOMIAL_ERROR_DID_NOT_CONVERGE){
            puts("AR_POLYNOMIAL_ERROR_DID_NOT_CONVERGE");
        }
        printf("polynomial:\n");
        int i;
        for (i = 0; i < 7; i++){
            printf("%e*x^%i", coefficients[i], 6 - i);
            if (i != 6) printf(" + ");
        }
        printf("\n");
        printf("\n");
        printf("ax = %f\n", ax);
        printf("ay = %f\n", ay);
        printf("bx = %f\n", bx);
        printf("by = %f\n", by);
        printf("cx = %f\n", cx);
        printf("cy = %f\n", cy);
        printf("dx = %f\n", dx);
        printf("dy = %f\n", dy);
        printf("center_x = %f\n", x);
        printf("center_y = %f\n", y);
        printf("radius = %f\n", radius);
        printf("\n");
        return -1;
    }

    *t = root;

    return 0;
}

struct ar_bezier3_dist_info {
    double t;
    double distance_squared;
    vec2 curve_point;
    vec2 other_point;
};

struct ar_bezier3_dist_info ar_bezier3_line_distance(
    const struct ar_bezier3 *curve,
    vec2 a, vec2 b
){
    struct ar_bezier3_dist_info info;
    info.t = AR_DBL_INF;
    info.distance_squared = -AR_DBL_INF;
    info.curve_point = v2(0.0, 0.0);
    info.other_point = v2(0.0, 0.0);

    int i, n = 100;
    for (i = 0; i < n; i++){
        double t = 1.0/(n - 1) * i;
        vec2 p = ar_bezier3_at(curve, t);
        vec2 ba = v2sub(b, a);
        double u = v2dot(v2sub(p, a), ba)/v2dot(ba, ba);
        if (u < 0.0) u = 0.0;
        if (u > 1.0) u = 1.0;
        vec2 q = v2lerp(a, b, u);
        double d = v2dist2(p, q);

        if (d > info.distance_squared){
            info.t = t;
            info.curve_point = p;
            info.other_point = q;
            info.distance_squared = d;
        }
    }

    return info;
}

struct ar_bezier3_dist_info ar_bezier3_arcs_distance(
    const struct ar_bezier3 *curve,
    const struct ar_arc *arcs
){
    /* TODO find better error measure */
    struct ar_bezier3_dist_info info;
    info.t = AR_DBL_INF;
    info.distance_squared = -AR_DBL_INF;
    info.curve_point = v2(0.0, 0.0);
    info.other_point = v2(0.0, 0.0);

    int i, n = 100;
    for (i = 0; i < n; i++){
        double t = 1.0/(n - 1) * i;
        vec2 p = ar_bezier3_at(curve, t);
        vec2 p0 = ar_arc_clamp(arcs + 0, p);
        vec2 p1 = ar_arc_clamp(arcs + 1, p);
        double d0 = v2dist2(p, p0);
        double d1 = v2dist2(p, p1);
        double d = d0 < d1 ? d0 : d1;

        if (d > info.distance_squared){
            info.t = t;
            info.curve_point = p;
            if (d0 < d1){
                info.other_point = p0;
                info.distance_squared = d0;
            }else{
                info.other_point = p1;
                info.distance_squared = d1;
            }
        }
    }

    return info;
}

struct ar_bezier3_dist_info ar_bezier3_arc_distance(
    const struct ar_bezier3 *curve,
    const struct ar_arc *arc
){
    /* TODO find better error measure */
    struct ar_bezier3_dist_info info;
    info.t = AR_DBL_INF;
    info.distance_squared = -AR_DBL_INF;
    info.curve_point = v2(0.0, 0.0);
    info.other_point = v2(0.0, 0.0);

    int i, n = 100;
    for (i = 0; i < n; i++){
        double t = 1.0/(n - 1) * i;
        vec2 p = ar_bezier3_at(curve, t);
        vec2 q = ar_arc_clamp(arc, p);
        double d = v2dist2(p, q);

        if (d > info.distance_squared){
            info.t = t;
            info.curve_point = p;
            info.other_point = q;
            info.distance_squared = d;
        }
    }

    return info;
}

#include <math.h>

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

void ar_arc_reverse(struct ar_arc *arc){
    AR_SWAP(vec2, arc->start, arc->end);
    arc->arc_type = !arc->arc_type;
}

#include "math/ar_arc_list.h"

void ar_fit(const struct ar_bezier3 *curve, double max_distance, int max_depth, struct ar_arc_list *output_arcs){
    const vec2 *p = curve->control_points;

    /* TODO scale input to [0, 1] and then back to input size */

    /* TODO split loops */

    vec2 a = p[0];
    vec2 b = p[1];
    vec2 c = p[2];
    vec2 d = p[3];

    vec2 ba = v2sub(b, a);
    vec2 cb = v2sub(c, b);
    vec2 cd = v2sub(c, d);

    double ba_len2  = v2len2(ba);
    double cb_len2  = v2len2(cb);
    double cd_len2  = v2len2(cd);

    double short_segment_threshold2 = 1e-20;

    int ba_is_short = ba_len2 < short_segment_threshold2;
    int cb_is_short = cb_len2 < short_segment_threshold2;
    int cd_is_short = cd_len2 < short_segment_threshold2;

    /* if control points form a line, output a line instead of an arc */
    if (v2dist_line(b, a, d) < short_segment_threshold2 && v2dist_line(c, a, d) < short_segment_threshold2){
        struct ar_arc arc[1];
        ar_arc_init(arc, v2(0.0, 0.0), 0.0, a, d, AR_ARC_LINE);
        ar_arc_list_add_tail(output_arcs, arc[0]);
        return;
    }

    if (cb_is_short){
        /* if the middle segment and any other segment is short, output line */
        if (ba_is_short || cd_is_short){
            struct ar_arc arc[1];
            ar_arc_init(arc, v2(0.0, 0.0), 0.0, a, d, AR_ARC_LINE);
            ar_arc_list_add_tail(output_arcs, arc[0]);

            return;
        }
    }else{
        /* middle segment is long */

        /* first and middle segment are short */
        if (ba_is_short && !cd_is_short){
            /* change control points from {a, b, c, d} to {a, c, c, d} */
            b = c;
            ba = v2sub(b, a);
            ba_len2 = v2len2(ba);
        }

        /* middle and last segment are short */
        if (!ba_is_short && cd_is_short){
            /* change control points from {a, b, c, d} to {a, b, b, d} */
            c = b;
            cd = v2sub(c, d);
            cd_len2 = v2len2(cd);
        }

        /* first and last but not middle segment are short */
        if (ba_is_short && cd_is_short){
            struct ar_arc arc[1];
            ar_arc_init(arc, v2(0.0, 0.0), 0.0, a, d, AR_ARC_LINE);
            ar_arc_list_add_tail(output_arcs, arc[0]);
            return;
        }
    }

#if 0
    /* TODO replace eps */
    double eps2 = 0.001;
    if (fabs(ba_len2 - cd_len2) < eps2){
#if 0
        /* TODO this arc is wrong */
        struct ar_arc arc[1];
        int clockwise = v2isright(d, a, b);
        vec2 normal = clockwise ? v2right(ba) : v2left(ba);/
        arc_from_points_and_normal(arc, a, d, normal, clockwise);
        arc->color = AR_YELLOW;
        ar_arc_list_add_tail(output_arcs, arc[0]);
        return;
#else
        /* split unevenly */
        struct ar_bezier3 curves[2];
        ar_bezier3_split(curve, 0.25, curves);
        ar_fit(curves + 0, max_distance, max_depth - 1, output_arcs);
        ar_fit(curves + 1, max_distance, max_depth - 1, output_arcs);
        return;
#endif
    }
#endif

    vec2 tangent_ba = v2smul(1.0/sqrt(ba_len2), ba);
    vec2 tangent_cd = v2smul(1.0/sqrt(cd_len2), cd);

    /* rotation matrix R = {{co, -si}, {si, co}} so that: R*tangent_ba = tangent_cd */
    double co = v2dot(tangent_ba, tangent_cd);
    double si = v2det(tangent_ba, tangent_cd);

    /* TODO replace eps */
    double eps = 0.00001;
    if (fabs(co - 1) < eps || fabs(co + 1) < eps){
        /*
        printf("WARNING: Tangent dangerously parallel\n");
        */

        struct ar_bezier3_dist_info info = ar_bezier3_line_distance(curve, a, d);

        if (max_depth > 0 && info.distance_squared > max_distance){
            struct ar_bezier3 curves[2];
            info.t = 0.5;
            ar_bezier3_split(curve, info.t, curves);
            ar_fit(curves + 0, max_distance, max_depth - 1, output_arcs);
            ar_fit(curves + 1, max_distance, max_depth - 1, output_arcs);
            return;
        }

        if (max_depth == 0){
            printf("WARNING: Maximum curve subdivisions reached. Fit may be worse than max_distance.\n");
            printf("tolerance exceeded by %f\n", sqrt(info.distance_squared));
        }

        struct ar_arc arc[1];
        ar_arc_init(arc, v2(0.0, 0.0), 0.0, a, d, AR_ARC_LINE);
        ar_arc_list_add_tail(output_arcs, arc[0]);
        return;
    }

    /* TODO handle co = -1 for tangents pointing in opposite directions. */

    /* find center of rotation so that: R*(a - center) + center = b */
    vec2 center;
    center.x = (a.x + d.x - si*(a.y - d.y)/(1.0 + co))*0.5;
    center.y = (a.y + d.y + si*(a.x - d.x)/(1.0 + co))*0.5;

    double radius = v2dist(a, center);

#if 0
    /* Tolerance not that important since join is projected onto circle. */
    /* Also this is just a heuristic anyway. */
    double root_t;
    int error = ar_bezier3_circle_intersection(a, b, c, d, center, radius, 1e-5, &root_t);
    if (error){
        printf("ba_len2 = %f\n", ba_len2);
        printf("cb_len2 = %f\n", cb_len2);
        printf("cd_len2 = %f\n", cd_len2);
        exit(-1);
    }
#endif
    /* TODO handle edge cases so bisection can work */
    double root_t = 0.5;

    vec2 join = ar_bezier3_at(curve, root_t);

    /* Ensure join really is on circle since intersection might be off. */
    join = v2add(center, v2scale(v2sub(join, center), radius));

    /* TODO can get rid of four additions here */
    int a_clockwise = v2isright(join, a, v2add(a, tangent_ba));
    int d_clockwise = v2isright(join, d, v2add(d, tangent_cd));

    vec2 normal_a = a_clockwise ? v2right(tangent_ba) : v2left(tangent_ba);
    vec2 normal_d = d_clockwise ? v2right(tangent_cd) : v2left(tangent_cd);

    struct ar_arc arcs[2];
    arc_from_points_and_normal(arcs + 0, a, join, normal_a, a_clockwise);
    arc_from_points_and_normal(arcs + 1, d, join, normal_d, d_clockwise);

    ar_arc_reverse(arcs + 1);

    struct ar_bezier3_dist_info info = ar_bezier3_arcs_distance(curve, arcs);

    if (max_depth > 0 && info.distance_squared > max_distance){
        struct ar_bezier3 curves[2];
        ar_bezier3_split(curve, info.t, curves);
        ar_fit(curves + 0, max_distance, max_depth - 1, output_arcs);
        ar_fit(curves + 1, max_distance, max_depth - 1, output_arcs);
        return;
    }

    ar_arc_list_add_tail(output_arcs, arcs[0]);
    ar_arc_list_add_tail(output_arcs, arcs[1]);

    if (max_depth == 0){
        printf("WARNING: Maximum curve subdivisions reached. Fit may be worse than max_distance.\n");
    }

    if (show_bezier){
        ar_bezier3_draw(curve, AR_WHITE);
    }

    if (show_control_points){
        ar_draw_points(control_points, 4, AR_RGB(100, 100, 100), GL_LINE_STRIP);

        ar_draw_arrow(a, p[1], 5.0, AR_GREEN);
        ar_draw_arrow(d, p[2], 5.0, AR_RED);
    }

    if (show_biarc){
        ar_draw_arc(arcs + 0, AR_GREEN);
        ar_draw_arc(arcs + 1, AR_RED);
    }

    if (show_solution_circle){
        ar_draw_circle(center, radius, AR_YELLOW);
    }
}

void ar_bezier3_fit_with_arcs(const struct ar_bezier3 *curve, struct ar_arc_list *arcs){
    ar_fit(curve, 0.1, 16, arcs);
}

struct ar_arc_list output_arcs[1];

void draw_line(double ax, double ay, double bx, double by){
    struct ar_arc arc[1];
    ar_arc_init(arc, v2(0.0, 0.0), 0.0, v2(ax, ay), v2(bx, by), AR_ARC_LINE);
    ar_arc_list_add_tail(output_arcs, arc[0]);
}

void draw_cubic(double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3){
    struct ar_bezier3 curve[1];
    ar_bezier3_init(curve, v2(x0, y0), v2(x1, y1), v2(x2, y2), v2(x3, y3));
    ar_bezier3_fit_with_arcs(curve, output_arcs);
}

void draw_quadratic(double x0, double y0, double x1, double y1, double x2, double y2){
    double bx = x0 + 2.0/3.0 * (x1 - x0);
    double by = y0 + 2.0/3.0 * (y1 - y0);
    double cx = x2 + 2.0/3.0 * (x1 - x2);
    double cy = y2 + 2.0/3.0 * (y1 - y2);
    draw_cubic(x0, y0, bx, by, cx, cy, x2, y2);
}

void ar_arc_vertices(const struct ar_arc *arc, struct ar_vertex *vertices, uint32_t color){
    vec2 a = arc->start;
    vec2 b = arc->end;
    vec2 center = arc->center;
    double radius = arc->radius;

    vec2 a_uv = v2smul(1.0/radius, v2sub(a, center));
    vec2 b_uv = v2smul(1.0/radius, v2sub(b, center));

    vec2 middle = v2lerp(a_uv, b_uv, 0.5);

    double scale = 1.0/v2len(middle);

    vec2 p = v2add(center, v2scale(a_uv, radius*scale));
    vec2 q = v2add(center, v2scale(b_uv, radius*scale));

    vertices[0] = ar_vert(a.x, a.y, a_uv.x      , a_uv.y      , color);
    vertices[1] = ar_vert(p.x, p.y, a_uv.x*scale, a_uv.y*scale, color);
    vertices[2] = ar_vert(b.x, b.y, b_uv.x      , b_uv.y      , color);

    vertices[3] = vertices[2];
    vertices[4] = vertices[1];
    vertices[5] = ar_vert(q.x, q.y, b_uv.x*scale, b_uv.y*scale, color);
}

#include "svg.h"

void on_frame(void){
    mat4 projection = m4_ortho2d(0.0f, window.width, window.height, 0.0f);
    mat23 world_to_screen = window.world_to_screen;
    mat4 modelview = m4m23(world_to_screen);
    mat4 mvp = m4mul(projection, modelview);

    AR_GL_CHECK
    ar_glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    ar_glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    AR_GL_CHECK

    window.width = glutGet(GLUT_WINDOW_WIDTH);
    window.height = glutGet(GLUT_WINDOW_HEIGHT);

    ar_shader_use(arc_shader);
    AR_GL_CHECK

    upload_model_view_projection(mvp);
    AR_GL_CHECK

    ar_glActiveTexture(GL_TEXTURE0);
    ar_texture_bind(texture);
    ar_glUniform1i(utex0, 0);

    ar_arc_list_init(output_arcs);

#if 0
    /* fit single curve */
    const vec2 *p = control_points;
    struct ar_bezier3 curve[1];
    ar_bezier3_init(curve, p[0], p[1], p[2], p[3]);
    ar_fit(curve, 0.1, 16, output_arcs);
#else
    /* fit multiple curves */
/*
#include "draw.h"
*/
    svg_parse_file("images/at_paragraph_snowman_thunder_cloud.svg");
#endif

    int n_arcs = output_arcs->n;

    printf("%i arcs\n", n_arcs);

    int n_vertices = n_arcs*(3 + 6);
    struct ar_vertex *vertices = malloc(n_vertices*sizeof(*vertices));
    struct ar_vertex *vertex_pointer = vertices;
    struct ar_arc_list_node *node;

    /* TODO split arcs longer than 180 degrees */

    vec2 pivot = v2(0.0, 0.0);

    for (node = output_arcs->head; node != NULL; node = node->next){
        struct ar_arc *arc = &node->value;

        vec2 a = arc->start;
        vec2 b = arc->end;

        *vertex_pointer++ = ar_vert(pivot.x, pivot.y, 0.0f, 0.0f, AR_GRAY);
        *vertex_pointer++ = ar_vert(a.x, a.y, 0.0f, 0.0f, AR_GRAY);
        *vertex_pointer++ = ar_vert(b.x, b.y, 0.0f, 0.0f, AR_GRAY);

        ar_arc_vertices(arc, vertex_pointer, AR_GREEN);
        vertex_pointer += 6;
    }

    /* prepare writing to stencil buffer */
    ar_glEnable(GL_STENCIL_TEST);
    ar_glClearStencil(0);
    ar_glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    ar_glStencilFunc(GL_NEVER, 0, 1);
    ar_glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);

    /* draw triangles */
    upload_model_view_projection(mvp);
    ar_draw(vertices, vertex_pointer - vertices, GL_TRIANGLES, apos, atex, acol);

    /* prepare coloring stencil buffer */
    ar_glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    ar_glStencilFunc(GL_EQUAL, 1, 1);
    ar_glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);

#if 0
    /* draw triangles again */
    upload_model_view_projection(mvp);
    ar_draw(vertices, vertex_pointer - vertices, GL_TRIANGLES, apos, atex, acol);
#else
    /* draw screen-filling rectangle */
    struct ar_vertex rect_vertices[2*3];
    ar_make_rect(rect_vertices, 0.0f, 0.0f, window.width, window.height, 0.0f, 0.0f, 0.0f, 0.0f, AR_GRAY);
    upload_model_view_projection(projection);
    ar_draw(rect_vertices, 2*3, GL_TRIANGLE_FAN, apos, atex, acol);
#endif
    ar_glDisable(GL_STENCIL_TEST);

#if 1
    /* draw polygon lines */
    vertex_pointer = vertices;
    for (node = output_arcs->head; node != NULL; node = node->next){
        struct ar_arc *arc = &node->value;

        vec2 a = arc->start;
        vec2 b = arc->end;

        *vertex_pointer++ = ar_vert(a.x, a.y, 0.0f, 0.0f, AR_GREEN);
        *vertex_pointer++ = ar_vert(b.x, b.y, 0.0f, 0.0f, AR_GREEN);
    }
    upload_model_view_projection(mvp);
    ar_draw(vertices, n_arcs*2, GL_LINES, apos, atex, acol);
#endif

    glutSwapBuffers();

    ar_arc_list_free(output_arcs);
    free(vertices);
}

void work(int frame){
    glutPostRedisplay();
    glutTimerFunc(20, work, frame + 1);
}

void on_move(int x, int y){
    vec2 old_world_mouse_pos, new_world_mouse_pos;
    window.mouse_pos_old = window.mouse_pos;

    old_world_mouse_pos = screen_to_world(window.mouse_pos);
    window.mouse_pos = v2(x, y);
    new_world_mouse_pos = screen_to_world(window.mouse_pos);

    if (window.is_mouse_button_down[GLUT_RIGHT_BUTTON]){
        vec2 world_delta = v2sub(new_world_mouse_pos, old_world_mouse_pos);

        mat23 t = window.world_to_screen;

        t = m23mul(t, m23(1.0, 0.0, world_delta.x, 0.0, 1.0, world_delta.y));

        window.world_to_screen = t;
    }

    if (window.is_mouse_button_down[GLUT_LEFT_BUTTON]){
        vec2 world_mouse_pos = screen_to_world(window.mouse_pos);
        vec2 *closest = NULL;

        int i;
        for (i = 0; i < 4; i++){
            if (!closest || v2dist2(control_points[i], world_mouse_pos) < v2dist2(*closest, world_mouse_pos)){
                closest = &control_points[i];
            }
        }

        *closest = world_mouse_pos;
    }
}

void on_scroll(double forward){

    double magnification = 8.0/7.0;
    double scale = pow(magnification, forward);

    vec2 pivot = screen_to_world(window.mouse_pos);

    mat23 t = window.world_to_screen;

    t = m23mul(t, m23(1.0, 0.0, +pivot.x, 0.0, 1.0, +pivot.y));
    t = m23mul(t, m23(scale, 0.0, 0.0, 0.0, scale, 0.0));
    t = m23mul(t, m23(1.0, 0.0, -pivot.x, 0.0, 1.0, -pivot.y));

    window.world_to_screen = t;
}

void on_button(int button, int action, int x, int y){
    int down = action == GLUT_DOWN;

    on_move(x, y);

    if (button == 3 && down) on_scroll(+1.0);
    if (button == 4 && down) on_scroll(-1.0);

    window.is_mouse_button_down[button] = down;
}

void on_key_down(unsigned char key, int x, int y){
    AR_UNUSED(x);
    AR_UNUSED(y);

    window.is_key_down[key] = 1;

    if (key == 'q') exit(0);
}

void on_key_up(unsigned char key, int x, int y){
    AR_UNUSED(x);
    AR_UNUSED(y);

    window.is_key_down[key] = 0;
}

int main(int argc, char **argv){
    const char *vert_src =
        "#version 120\r\n"
        AR_STR(
        attribute vec4 apos;
        attribute vec2 atex;
        attribute vec4 acol;

        varying vec2 vtex;
        varying vec4 vcol;

        uniform mat4 umvp;

        void main(){
            vtex = atex;
            vcol = acol;
            gl_Position = umvp*vec4(apos.xyz, 1.0);
        }
    );

    const char *frag_src =
        "#version 120\r\n"
        AR_STR(
        precision highp float;

        varying vec2 vtex;
        varying vec4 vcol;

        uniform sampler2D utex0;

        void main(){
            float r = length(vtex);

            if (r > 1.0) discard;

            gl_FragColor = texture2D(utex0, vtex) * vcol;
        }
    );

    int x, y;
    int nx = 16;
    int ny = 16;
    uint32_t texture_data[16*16];
    for (y = 0; y < ny; y++) for (x = 0; x < nx; x++){
        texture_data[x + y*nx] =  0xffffffff;
    }

    window.width = 512;
    window.height = 512;
    window.world_to_screen = m23id();

    control_points[0] = v2(200, 100);
    control_points[1] = v2(200, 200);
    control_points[2] = v2(200, 200);
    control_points[3] = v2(100, 200);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(window.width, window.height);
    glutCreateWindow("");

    ar_gl_init();

    ar_shader_init(arc_shader, vert_src, frag_src);

    umvp  = ar_glGetUniformLocation(arc_shader->program, "umvp");
    utex0 = ar_glGetUniformLocation(arc_shader->program, "utex0");
    apos  = ar_glGetAttribLocation(arc_shader->program, "apos");
    atex  = ar_glGetAttribLocation(arc_shader->program, "atex");
    acol  = ar_glGetAttribLocation(arc_shader->program, "acol");

    assert(apos != -1);
    assert(atex != -1);
    assert(acol != -1);
    assert(umvp != -1);
    assert(utex0 != -1);

    ar_texture_init(texture, nx, ny, texture_data);

    glutCreateMenu(menu_callback);
    glutAddMenuEntry("Show bezier", 0);
    glutAddMenuEntry("Control points", 4);
    glutAddMenuEntry("Biarc", 5);
    glutAddMenuEntry("Solution circle", 1);
    glutAddMenuEntry("Max distance", 3);
    glutAttachMenu(GLUT_MIDDLE_BUTTON);

    glutMotionFunc(on_move);
    glutPassiveMotionFunc(on_move);
    glutMouseFunc(on_button);
    glutKeyboardFunc(on_key_down);
    glutKeyboardUpFunc(on_key_up);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glutDisplayFunc(on_frame);
    work(0);
    glutMainLoop();
    return 0;
}
