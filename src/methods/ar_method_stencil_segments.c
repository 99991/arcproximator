#include "ar_method_stencil_segments.h"
#include "../math/vec2.h"
#include "../math/ar_bezier3.h"
#include "../graphics/ar_vertex.h"
#include "../graphics/ar_draw.h"
#include "../graphics/ar_color.h"

#include <math.h>
#include <stdio.h>
#include <assert.h>

static int n_vertices;
static struct ar_vertex *vertices;
static float bounds_x0;
static float bounds_y0;
static float bounds_x1;
static float bounds_y1;

void prepare_stencil_segments(const char *path){
    FILE *fp = fopen(path, "rb");
    assert(fp);
    int j, m;
    fscanf(fp, "%d", &m);

    /* probably less, but memory is cheap */
    n_vertices = m*256*3 + 2*3;
    vertices = malloc(n_vertices*sizeof(*vertices));
    struct ar_vertex *vertex_pointer = vertices;
    struct ar_bezier3 *curves = malloc(m*sizeof(*curves));

    for (j = 0; j < m; j++){
        float ax, ay, bx, by, cx, cy, dx, dy;
        fscanf(fp, "%f %f %f %f %f %f %f %f", &ax, &ay, &bx, &by, &cx, &cy, &dx, &dy);
        ar_bezier3_init(&curves[j], v2(ax, ay), v2(bx, by), v2(cx, cy), v2(dx, dy));
    }

    vec2 p = curves[0].control_points[0];

    bounds_x0 = bounds_x1 = p.x;
    bounds_y0 = bounds_y1 = p.y;

    for (j = 0; j < m; j++){
        int n = 256;
        vec2 points[n];
        n = ar_bezier3_subdivide(&curves[j], points, n, ar_deg2rad(2.0));
        int i;
        vec2 a = points[0];
        for (i = 1; i < n; i++){
            vec2 b = points[i];
            *vertex_pointer++ = ar_vert(p.x, p.y, 0.0f, 0.0f, AR_BLACK);
            *vertex_pointer++ = ar_vert(a.x, a.y, 0.0f, 0.0f, AR_BLACK);
            *vertex_pointer++ = ar_vert(b.x, b.y, 0.0f, 0.0f, AR_BLACK);
            a = b;
        }
    }

    free(curves);

    n_vertices = vertex_pointer - vertices;

    int i;
    for (i = 0; i < n_vertices; i++){
        float x = vertices[i].x;
        float y = vertices[i].y;
        if (x < bounds_x0) bounds_x0 = x;
        if (y < bounds_y0) bounds_y0 = y;
        if (x > bounds_x1) bounds_x1 = x;
        if (y > bounds_y1) bounds_y1 = y;
    }

    ar_make_rect(
        vertex_pointer,
        bounds_x0, bounds_y0,
        bounds_x1, bounds_y1,
        0.0f, 0.0f, 0.0f, 0.0f,
        AR_BLACK);
#if 0
    double area = 0.0;
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

void upload_stencil_segments(struct ar_shader *shader, GLuint vbo){
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(*vertices)*(n_vertices + 2*3), vertices);
    ar_set_attributes(shader, vertices);
}

void render_stencil_segments(void){
    glEnable(GL_STENCIL_TEST);
    glClearStencil(0);

    /* prepare writing to stencil buffer */
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glStencilFunc(GL_NEVER, 0, 1);
    glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);

    glDrawArrays(GL_TRIANGLES, 0, n_vertices);

    /* prepare coloring stencil buffer */
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilFunc(GL_EQUAL, 1, 1);
    glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);

    glDrawArrays(GL_TRIANGLES, n_vertices, 2*3);

    glDisable(GL_STENCIL_TEST);
}
