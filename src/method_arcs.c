#include "method_arcs.h"
#include "graphics/ar_draw.h"
#include "graphics/ar_color.h"
#include "graphics/ar_shader.h"
#include "graphics/ar_vertex.h"
#include "math/ar_arc.h"

#include <assert.h>
#include <stdio.h>

static int n_vertices;
static struct ar_vertex *vertices;

void prepare_arcs_renderer(const char *path){
    int i, n;
    FILE *fp = fopen(path, "rb");
    assert(fp);
    fscanf(fp, "%d", &n);

    assert(n % 2 == 0);

    n_vertices = n/2*6;

    struct ar_arc *arcs = malloc(sizeof(*arcs)*n);
    vertices = malloc(sizeof(*vertices)*n_vertices);

    for (i = 0; i < n; i++){
        struct ar_arc *arc = &arcs[i];
        int type;
        fscanf(fp, "%d", &type);
        if (type == 0){
            float cx, cy, x0, y0, x1, y1;
            int clockwise;
            fscanf(fp, "%f %f %f %f %f %f %d", &cx, &cy, &x0, &y0, &x1, &y1, &clockwise);
            vec2 center = v2(cx, cy);
            vec2 a = v2(x0, y0);
            vec2 b = v2(x1, y1);
            double radius = v2dist(a, center);
            ar_arc_init(arc, center, radius, a, b, clockwise ? AR_ARC_CLOCKWISE : AR_ARC_COUNTERCLOCKWISE);
        }else{
            float x0, y0, x1, y1;
            fscanf(fp, "%f %f %f %f", &x0, &y0, &x1, &y1);
            vec2 a = v2(x0, y0);
            vec2 b = v2(x1, y1);
            ar_arc_init(arc, a, 0.0, a, b, AR_ARC_LINE);
        }
    }

    fclose(fp);

    struct ar_vertex *vertex_ptr = vertices;

    for (i = 0; i < n; i += 2){
        struct ar_arc *lower = &arcs[i + 0];
        struct ar_arc *upper = &arcs[i + 1];

        struct ar_vertex v = ar_vert(0.0f, 0.0f, 0.0f, 0.0f, AR_BLACK);

        v.cx_lower = lower->center.x;
        v.cy_lower = lower->center.y;
        v.cx_upper = upper->center.x;
        v.cy_upper = upper->center.y;
        v.r_lower = lower->radius;
        v.r_upper = upper->radius;
        v.type_lower = lower->arc_type;
        v.type_upper = upper->arc_type;

        vec2 p[4] = {
            v2(lower->start.x, lower->start.y),
            v2(lower->end.x, lower->end.y),
            v2(upper->end.x, upper->end.y),
            v2(upper->start.x, upper->start.y),
        };

        /* arcs must be x-monotone */
        /* and the lower arc must be below the upper arc */
        assert(p[0].x == p[3].x);
        assert(p[1].x == p[2].x);
        assert(p[0].y <= p[3].y);
        assert(p[1].y <= p[2].y);
        assert(p[0].x < p[1].x);
        assert(p[3].x < p[2].x);

        if (lower->arc_type == AR_ARC_COUNTERCLOCKWISE){
            vec2 c = lower->center;
            float y = p[0].x <= c.x && c.x <= p[1].x ? c.y - lower->radius :
                p[0].y < p[1].y ? p[0].y : p[1].y;
            p[0].y = y;
            p[1].y = y;
        }

        if (upper->arc_type == AR_ARC_CLOCKWISE){
            vec2 c = upper->center;
            float y = p[0].x <= c.x && c.x <= p[1].x ? c.y + upper->radius :
                p[2].y > p[3].y ? p[2].y : p[3].y;
            p[2].y = y;
            p[3].y = y;
        }

        int j, indices[6] = {0, 1, 2, 0, 2, 3};
        for (j = 0; j < 6; j++){
            int k = indices[j];
            v.x = p[k].x;
            v.y = p[k].y;
            *vertex_ptr++ = v;
        }
    }
    free(arcs);
}

void upload_arcs(struct ar_shader *shader, GLuint vbo){
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(*vertices)*n_vertices, vertices);
    ar_set_attributes(shader, vertices);
}

void render_arcs(void){
    glDrawArrays(GL_TRIANGLES, 0, n_vertices);
}
