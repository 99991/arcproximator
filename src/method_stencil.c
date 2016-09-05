#include "method_stencil.h"
#include "math/ar_arc.h"
#include "math/ar_arc_list.h"
#include "math/vec2.h"
#include "math/ar_bezier3.h"
#include "graphics/ar_vertex.h"
#include "graphics/ar_draw.h"

#include <math.h>
#include <stdio.h>

static int n_vertices;
static struct ar_vertex *vertices;

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

void ar_fit(const struct ar_bezier3 *curve, double max_distance, int max_depth, struct ar_arc_list *output_arcs){
    const vec2 *p = curve->control_points;

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

    vec2 tangent_ba = v2smul(1.0/sqrt(ba_len2), ba);
    vec2 tangent_cd = v2smul(1.0/sqrt(cd_len2), cd);

    /* rotation matrix R = {{co, -si}, {si, co}} so that: R*tangent_ba = tangent_cd */
    double co = v2dot(tangent_ba, tangent_cd);
    double si = v2det(tangent_ba, tangent_cd);

    double eps = 0.00001;
    if (fabs(co - 1) < eps || fabs(co + 1) < eps){

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

    if (max_depth > 0 && sqrt(info.distance_squared) > max_distance){
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

void ar_arc_vertices(const struct ar_arc *arc, struct ar_vertex *out_vertices, uint32_t color){
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

    out_vertices[0] = ar_vert(a.x, a.y, a_uv.x      , a_uv.y      , color);
    out_vertices[1] = ar_vert(p.x, p.y, a_uv.x*scale, a_uv.y*scale, color);
    out_vertices[2] = ar_vert(b.x, b.y, b_uv.x      , b_uv.y      , color);

    out_vertices[3] = out_vertices[2];
    out_vertices[4] = out_vertices[1];
    out_vertices[5] = ar_vert(q.x, q.y, b_uv.x*scale, b_uv.y*scale, color);
}

#include "svg.h"

void prepare_svg(const char *path){
    ar_arc_list_init(output_arcs);

    svg_parse_file(path);

    int n_arcs = output_arcs->n;

    n_vertices = n_arcs*(3 + 6);
    vertices = malloc(n_vertices*sizeof(*vertices));
    struct ar_vertex *vertex_pointer = vertices;
    struct ar_arc_list_node *node;

    vec2 pivot = output_arcs->head->value.start;

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

    int vertices_written = vertex_pointer - vertices;
    assert(n_vertices == vertices_written);

    ar_arc_list_free(output_arcs);

#if 0
    double area = 0.0;
    int j;
    for (j = 0; j < n_vertices; j += 3){
        struct ar_vertex *v = vertices + j;
        vec2 a = v2(v[0].x, v[0].y);
        vec2 b = v2(v[1].x, v[1].y);
        vec2 c = v2(v[2].x, v[2].y);
        double triangle_area = fabs(0.5*v2det(v2sub(b, a), v2sub(c, a)));
        if (triangle_area == triangle_area){
            area += triangle_area;
        }
    }
    printf("area: %f, %f %% covered\n", area, area*100.0/800/800);
#endif
}

void upload_svg(struct ar_shader *shader, GLuint vbo){
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(*vertices)*n_vertices, vertices);
    ar_set_attributes(shader, vertices);
}

void render_svg(struct ar_shader *shader, GLuint vbo, mat4 mvp, mat4 projection){
    GL_CHECK
    ar_shader_use(shader);

    GL_CHECK
    glEnable(GL_STENCIL_TEST);
    glClearStencil(0);

    GL_CHECK
    /* prepare writing to stencil buffer */
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glStencilFunc(GL_NEVER, 0, 1);
    glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);

    GL_CHECK
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    ar_set_attributes(shader, NULL);

    GL_CHECK
    ar_upload_model_view_projection(shader, mvp);
    glDrawArrays(GL_TRIANGLES, 0, n_vertices);

    GL_CHECK
    /* prepare coloring stencil buffer */
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilFunc(GL_EQUAL, 1, 1);
    glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);

    GL_CHECK
    ar_upload_model_view_projection(shader, projection);
    struct ar_vertex rect_vertices[2*3];
    ar_make_rect(rect_vertices, 0.0f, 0.0f, 4096.0f, 4096.0f, 0.0f, 0.0f, 0.0f, 0.0f, AR_BLACK);
    ar_draw(shader, rect_vertices, 2*3, GL_TRIANGLES, vbo);

    GL_CHECK
    glDisable(GL_STENCIL_TEST);
    GL_CHECK
}
