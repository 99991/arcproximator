#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "math/ar_bezier3.h"
#include "graphics/ar_shader.h"
#include "graphics/ar_draw.h"
#include "graphics/ar_color.h"

#include <GL/glu.h>

#define MAX_COORDS (1024*1024)
static GLdouble coords[MAX_COORDS];
static int coords_index = 0;
static struct ar_vertex tess_vertices[MAX_COORDS];
static int tess_vertex_index = 0;

double angle_between(vec2 v, vec2 w){
    v = v2normalize(v);
    w = v2normalize(w);
    return acos(v2dot(v, w));
}

double deg2rad(double degrees){
    return AR_PI/180.0 * degrees;
}

int subdivide(struct ar_bezier3 *curve, vec2 *points, int n_max){
    vec2 *p = curve->control_points;
    vec2 v = v2sub(p[1], p[0]);
    int i;
    double t = 0.0;
    double eps = 0.001;
    vec2 a = p[0];
    points[0] = a;
    for (i = 1; i < n_max - 1; i++){
        while (t < 1.0){
            t += eps;

            vec2 b = ar_bezier3_at(curve, t);
            vec2 w = v2sub(b, a);

            if (angle_between(v, w) > deg2rad(2.0)){
                points[i] = a;
                a = b;
                v = w;
                break;
            }
        }
        if (t >= 1.0){
            break;
        }
    }
    points[i++] = p[3];

    return i;
}

void CALLBACK begin_callback(GLenum mode){
    assert(mode == GL_TRIANGLES);
}

void CALLBACK end_callback(void){

}

void CALLBACK vertex_callback(const GLvoid *data){
    const GLdouble *p = (const GLdouble*)data;
    tess_vertices[tess_vertex_index++] = ar_vert(p[0], p[1], 0.0f, 0.0f, AR_BLACK);
}

void CALLBACK combine_callback(
    const GLdouble new_vertex[3],
    const GLdouble *neighbor_vertex[4],
    const GLfloat neighbor_weight[4],
    GLdouble **out
){
    AR_UNUSED(neighbor_vertex);
    AR_UNUSED(neighbor_weight);
    GLdouble *p = &coords[coords_index];
    coords_index += 3;
    assert(coords_index <= MAX_COORDS);
    p[0] = new_vertex[0];
    p[1] = new_vertex[1];
    p[2] = new_vertex[2];
    *out = p;
}

void CALLBACK error_callback(GLenum error){
    printf("%s\n", (const char*)gluErrorString(error));
}

void CALLBACK tess_edge_callback(GLboolean flag){
    AR_UNUSED(flag);
}

void prepare_segments(const char *path){

    coords_index = 0;
    tess_vertex_index = 0;

    GLUtesselator *tess = gluNewTess();

    gluTessCallback(tess, GLU_TESS_BEGIN,   (void (CALLBACK *)())begin_callback);
    gluTessCallback(tess, GLU_TESS_END,     (void (CALLBACK *)())end_callback);
    gluTessCallback(tess, GLU_TESS_VERTEX,  (void (CALLBACK *)())vertex_callback);
    gluTessCallback(tess, GLU_TESS_COMBINE, (void (CALLBACK *)())combine_callback);
    gluTessCallback(tess, GLU_TESS_ERROR,   (void (CALLBACK *)())error_callback);
    gluTessCallback(tess, GLU_TESS_EDGE_FLAG, (void (CALLBACK *)())tess_edge_callback);

    gluTessBeginPolygon(tess, NULL);
    gluTessBeginContour(tess);

    FILE *fp = fopen(path, "rb");
    assert(fp);
    int j, m;
    fscanf(fp, "%d", &m);
    for (j = 0; j < m; j++){
        float ax, ay, bx, by, cx, cy, dx, dy;
        fscanf(fp, "%f %f %f %f %f %f %f %f", &ax, &ay, &bx, &by, &cx, &cy, &dx, &dy);
        struct ar_bezier3 curve[1];
        ar_bezier3_init(curve, v2(ax, ay), v2(bx, by), v2(cx, cy), v2(dx, dy));

        int n = 256;
        vec2 points[n];
        n = subdivide(curve, points, n);
        int i;
        for (i = 0; i < n; i++){
            int index = coords_index;
            coords[coords_index++] = points[i].x;
            coords[coords_index++] = points[i].y;
            coords[coords_index++] = 0.0;
            gluTessVertex(tess, &coords[index], &coords[index]);
        }
    }

    gluTessEndContour(tess);
    gluTessEndPolygon(tess);
    gluDeleteTess(tess);
}

void upload_segments(struct ar_shader *shader, GLuint vbo){
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    struct ar_vertex *vertices = tess_vertices;
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(*vertices)*tess_vertex_index, vertices);
    ar_set_attributes(shader, vertices);
}

void render_segments(void){
    glDrawArrays(GL_TRIANGLES, 0, tess_vertex_index);
}
