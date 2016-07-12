#include "math/ar_bezier3.h"
#include "math/ar_arc.h"
#include "graphics/ar_shader.h"
#include "graphics/ar_draw.h"
#include "graphics/ar_texture.h"
#include "graphics/ar_color.h"
#include "math/vec2.h"
#include "math/mat23.h"
#include "math/mat4.h"

#include <assert.h>
#include <stdio.h>

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

void make_rect(
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

void upload_model_view_projection(mat4 mvp, GLint umvp){
    /* upload as double not available everywhere */
    float data[16];
    int i, j;
    for (i = 0; i < 4; i++){
        for (j = 0; j < 4; j++){
            /* column major matrix */
            data[i + j*4] = m4at(mvp, i, j);
        }
    }
    glUniformMatrix4fv(umvp, 1, 0, data);
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

void ar_bezier3_draw(const struct ar_bezier3 *curve, uint32_t color){
    int n = 100;
    vec2 *points = (vec2*)malloc(sizeof*(points)*n);
    ar_bezier3_points(curve, points, n, 0.0, 1.0);
    ar_draw_points(points, n, color, GL_LINE_STRIP);
    free(points);
}

void arc_from_points_and_normal(struct ar_arc *arc, vec2 start, vec2 end, vec2 start_normal, int clockwise){
    vec2 v = v2sub(end, start);
    double radius = v2dot(v, v)*0.5/v2dot(v, start_normal);
    AR_ASSERT_GOOD_NUMBER(radius);
    vec2 center = v2add(start, v2smul(radius, start_normal));

    ar_arc_init(arc, center, radius, start, end, clockwise);
}

void ar_circle_points(vec2 *points, int n, vec2 center, double radius){
    double delta_angle = 2.0*AR_PI/n;
    int i;
    for (i = 0; i < n; i++){
        double angle = i*delta_angle;
        points[i] = v2add(center, v2polar(angle, radius));
    }
}

vec2 points[100];

void ar_draw_circle(vec2 center, double radius, uint32_t color){
    ar_circle_points(points, 100, center, radius);
    ar_draw_points(points, 100, color, GL_LINE_LOOP);
}

void ar_draw_arrow(vec2 a, vec2 b, double r, uint32_t color){
    points[0] = a;
    points[1] = b;
    ar_draw_points(points, 2, color, GL_LINES);

    vec2 d = v2scale(v2sub(b, a), r);
    points[0] = b;
    points[1] = v2sub(b, v2add(v2smul(2.0, d), v2left(d)));
    points[2] = v2sub(b, v2sub(v2smul(2.0, d), v2left(d)));

    ar_draw_points(points, 3, color, GL_TRIANGLES);
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
int show_solution_circle  = 1;
int show_useful_solutions = 0;
int show_max_dist         = 0;
int show_control_points   = 1;
int show_biarc            = 1;

void menu_callback(int option){
    switch (option){
        case 1: show_solution_circle  = !show_solution_circle;  break;
        case 2: show_useful_solutions = !show_useful_solutions; break;
        case 3: show_max_dist         = !show_max_dist;         break;
        case 4: show_control_points   = !show_control_points;   break;
        case 5: show_biarc            = !show_biarc;            break;
    }
}

#include "math/ar_durand_kerner_roots.h"

#define AR_BEZIER3_CIRCLE_ROOTS_COUNT 6

int ar_bezier3_circle_roots(
    const struct ar_bezier3 *curve,
    vec2 center,
    double radius,
    vec2 *roots
){
    const vec2 *p = curve->control_points;
    double ax = p[0].x;
    double ay = p[0].y;
    double bx = p[1].x;
    double by = p[1].y;
    double cx = p[2].x;
    double cy = p[2].y;
    double dx = p[3].x;
    double dy = p[3].y;
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

    double coeffs[7] = {
        (ax*ax) - 6*ax*bx + 6*ax*cx - 2*ax*dx + (ay*ay) - 6*ay*by + 6*ay*cy - 2*ay*dy + 9*(bx*bx) - 18*bx*cx + 6*bx*dx + 9*(by*by) - 18*by*cy + 6*by*dy + 9*(cx*cx) - 6*cx*dx + 9*(cy*cy) - 6*cy*dy + (dx*dx) + (dy*dy),
        -6*(ax*ax) + 30*ax*bx - 24*ax*cx + 6*ax*dx - 6*(ay*ay) + 30*ay*by - 24*ay*cy + 6*ay*dy - 36*(bx*bx) + 54*bx*cx - 12*bx*dx - 36*(by*by) + 54*by*cy - 12*by*dy - 18*(cx*cx) + 6*cx*dx - 18*(cy*cy) + 6*cy*dy,
        15*(ax*ax) - 60*ax*bx + 36*ax*cx - 6*ax*dx + 15*(ay*ay) - 60*ay*by + 36*ay*cy - 6*ay*dy + 54*(bx*bx) - 54*bx*cx + 6*bx*dx + 54*(by*by) - 54*by*cy + 6*by*dy + 9*(cx*cx) + 9*(cy*cy),
        -20*(ax*ax) + 60*ax*bx - 24*ax*cx + 2*ax*dx + 2*ax*x - 20*(ay*ay) + 60*ay*by - 24*ay*cy + 2*ay*dy + 2*ay*y - 36*(bx*bx) + 18*bx*cx - 6*bx*x - 36*(by*by) + 18*by*cy - 6*by*y + 6*cx*x + 6*cy*y - 2*dx*x - 2*dy*y,
        15*(ax*ax) - 30*ax*bx + 6*ax*cx - 6*ax*x + 15*(ay*ay) - 30*ay*by + 6*ay*cy - 6*ay*y + 9*(bx*bx) + 12*bx*x + 9*(by*by) + 12*by*y - 6*cx*x - 6*cy*y,
        -6*(ax*ax) + 6*ax*bx + 6*ax*x - 6*(ay*ay) + 6*ay*by + 6*ay*y - 6*bx*x - 6*by*y,
        (ax*ax) - 2*ax*x + (ay*ay) - 2*ay*y - (r*r) + (x*x) + (y*y),
    };

    /* TODO better root finding method */
    int iterations = ar_durand_kerner_roots(coeffs, 7, roots, 1000, 1e-15);

    if (iterations < 0){
        printf("ERROR: durand kerner method failed to converge\n");
        /* TODO remove this */
        exit(-1);
        return iterations;
    }

    int i;
    for (i = 0; i < AR_BEZIER3_CIRCLE_ROOTS_COUNT; i++){
        vec2 root = roots[i];
        AR_ASSERT_GOOD_NUMBER(root.x);
        AR_ASSERT_GOOD_NUMBER(root.y);
        vec2 p = ar_polynomial_eval_complex(coeffs, 7, roots[i]);
        if (v2len(p) > 1e-4){
            printf("WARNING: root %f %e %e is not very close to zero: %e %e\n", v2len(p), root.x, root.y, p.x, p.y);
        }
    }

    return iterations;
}

void ar_fit(const struct ar_bezier3 *curve){
    /* TODO split loops */
    const vec2 *p = curve->control_points;

    vec2 a = p[0];
    vec2 b = p[3];
    vec2 tangent_a = v2normalize(v2sub(p[1], a));
    vec2 tangent_b = v2normalize(v2sub(p[2], b));
    AR_ASSERT_GOOD_NUMBER(tangent_a.x);
    AR_ASSERT_GOOD_NUMBER(tangent_a.y);
    AR_ASSERT_GOOD_NUMBER(tangent_b.x);
    AR_ASSERT_GOOD_NUMBER(tangent_b.y);

    /* rotation matrix R = {{c, -s}, {s, c}} so that: R*tangent_a = tangent_b */
    double c = v2dot(tangent_a, tangent_b);
    double s = v2det(tangent_a, tangent_b);

    /* find center of rotation so that: R*(a - center) + center = b */
    vec2 center;
    center.x = (a.x + b.x - s*(a.y - b.y)/(1.0 + c))*0.5;
    center.y = (a.y + b.y + s*(a.x - b.x)/(1.0 + c))*0.5;
    AR_ASSERT_GOOD_NUMBER(center.x);
    AR_ASSERT_GOOD_NUMBER(center.y);

    double radius = v2dist(a, center);

    vec2 roots[AR_BEZIER3_CIRCLE_ROOTS_COUNT];
    ar_bezier3_circle_roots(curve, center, radius, roots);
    double root_t = 0.0;
    double root_t_dist = AR_DBL_INF;
    /* find the root that is closest to t = 0.5 */
    int i;
    for (i = 0; i < AR_BEZIER3_CIRCLE_ROOTS_COUNT; i++){
        vec2 root = roots[i];
        if (fabs(root.y) < 1e-5){
            double t = root.x;
            double t_dist = fabs(0.5 - t);
            if (t_dist < root_t_dist){
                root_t_dist = t_dist;
                root_t = t;
            }
        }
    }

    if (root_t_dist == AR_DBL_INF){
        printf("ERROR: didn't find root\n");
        return;
    }

    vec2 join = ar_bezier3_at(curve, root_t);

    if (join.x == a.x && join.y == a.y){
        printf("ERROR: join == a\n");
        return;
    }

    if (join.x == b.x && join.y == b.y){
        printf("ERROR: join == b\n");
        return;
    }

    int a_clockwise = v2isright(join, a, v2add(a, tangent_a));
    int b_clockwise = v2isright(join, b, v2add(b, tangent_b));

    vec2 normal_a = a_clockwise ? v2right(tangent_a) : v2left(tangent_a);
    vec2 normal_b = b_clockwise ? v2right(tangent_b) : v2left(tangent_b);
    AR_ASSERT_GOOD_NUMBER(normal_a.x);
    AR_ASSERT_GOOD_NUMBER(normal_a.y);
    AR_ASSERT_GOOD_NUMBER(normal_b.x);
    AR_ASSERT_GOOD_NUMBER(normal_b.y);

    struct ar_arc arcs[2];
    arc_from_points_and_normal(arcs + 0, a, join, normal_a, a_clockwise);
    arc_from_points_and_normal(arcs + 1, b, join, normal_b, b_clockwise);

    if (show_control_points){
        ar_draw_points(control_points, 4, AR_RGB(100, 100, 100), GL_LINE_STRIP);

        ar_draw_arrow(a, p[1], 5.0, AR_GREEN);
        ar_draw_arrow(b, p[2], 5.0, AR_RED);
    }

    ar_bezier3_draw(curve, AR_WHITE);

    if (show_biarc){
        ar_arc_points(arcs + 0, points, 100, 0.0, 1.0);
        ar_draw_points(points, 100, AR_GREEN, GL_LINE_STRIP);
        ar_arc_points(arcs + 1, points, 100, 0.0, 1.0);
        ar_draw_points(points, 100, AR_RED, GL_LINE_STRIP);
    }

    if (show_solution_circle){
        ar_draw_circle(center, radius, AR_YELLOW);
    }

    if (show_useful_solutions){
        int aright = v2isright(a, p[1], p[2]);
        int bright = v2isright(b, p[1], p[2]);
        int positive = s > 0.0;
        int clockwise = aright != bright ? /* S */ positive : /* U */ aright;

        struct ar_arc arc[1];
        ar_arc_init(arc, center, radius, a, b, clockwise);

        glLineWidth(3.0f);
        ar_arc_points(arc, points, 100, 0.0, 1.0);
        ar_draw_points(points, 100, AR_LIGHT_BLUE, GL_LINE_STRIP);
        glLineWidth(1.0f);
    }
}

void on_frame(){
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    window.width = glutGet(GLUT_WINDOW_WIDTH);
    window.height = glutGet(GLUT_WINDOW_HEIGHT);

    ar_shader_use(arc_shader);

    AR_GL_CHECK

    mat4 projection = m4_ortho2d(0.0f, window.width, 0.0f, window.height);

    AR_GL_CHECK

    mat23 world_to_screen = window.world_to_screen;

    mat4 modelview = m4m23(world_to_screen);

    mat4 mvp = m4mul(projection, modelview);

    upload_model_view_projection(mvp, umvp);

    AR_GL_CHECK

    glActiveTexture(GL_TEXTURE0);
    ar_texture_bind(texture);
    glUniform1i(utex0, 0);

    struct ar_bezier3 curve[1];
    const vec2 *p = control_points;
    ar_bezier3_init(curve, p[0], p[1], p[2], p[3]);
    ar_fit(curve);

    /*
    TODO
    stencil polygon
    {
        glDisable(GL_CULL_FACE);

        glEnable(GL_STENCIL_TEST);
        glClearStencil(0);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glStencilFunc(GL_NEVER, 0, 1);
        glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);

        draw(poly.data(), poly.size(), GL_TRIANGLE_FAN);

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glStencilFunc(GL_EQUAL, 1, 1);
        glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);

        float quad[4*2] = {
            0.0f, 0.0f,
            (float)w, 0.0f,
            (float)w, (float)h,
            0.0f, (float)h
        };
        glColor3f(0, 1, 0);
        draw(quad, 4, GL_QUADS);
        glDisable(GL_STENCIL_TEST);
    }
    */

    glutSwapBuffers();
}

void work(int frame){
    glutPostRedisplay();
    glutTimerFunc(20, work, frame + 1);
}

void on_move(int x, int y){
    window.mouse_pos_old = window.mouse_pos;

    vec2 old_world_mouse_pos = screen_to_world(window.mouse_pos);
    window.mouse_pos = v2(x, window.height - 1 - y);
    vec2 new_world_mouse_pos = screen_to_world(window.mouse_pos);

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
    on_move(x, y);

    int down = action == GLUT_DOWN;

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
    window.width = 512;
    window.height = 512;
    window.world_to_screen = m23id();

    control_points[0] = v2(100, 100);
    control_points[1] = v2(500, 150);
    control_points[2] = v2(50, 300);
    control_points[3] = v2(400, 500);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(window.width, window.height);
    glutCreateWindow("");

    glewInit();

    const char *vert_src = AR_STR(
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

    const char *frag_src = AR_STR(
        varying vec2 vtex;
        varying vec4 vcol;

        uniform sampler2D utex0;

        void main(){
            if (length(vtex) > 1.0) discard;

            gl_FragColor = texture2D(utex0, vtex) * vcol;
        }
    );

    ar_shader_init(arc_shader, vert_src, frag_src);

    umvp  = glGetUniformLocation(arc_shader->program, "umvp");
    utex0 = glGetUniformLocation(arc_shader->program, "utex0");
    apos  = glGetAttribLocation(arc_shader->program, "apos");
    atex  = glGetAttribLocation(arc_shader->program, "atex");
    acol  = glGetAttribLocation(arc_shader->program, "acol");

    assert(apos != -1);
    assert(atex != -1);
    assert(acol != -1);
    assert(umvp != -1);
    assert(utex0 != -1);

    int x, y;
    int nx = 16;
    int ny = 16;
    uint32_t data[16*16];
    for (y = 0; y < ny; y++) for (x = 0; x < nx; x++){
        data[x + y*nx] =  0xffffffff;
    }
    ar_texture_init(texture, nx, ny, data);

    glutCreateMenu(menu_callback);
    glutAddMenuEntry("Control points", 4);
    glutAddMenuEntry("Biarc", 5);
    glutAddMenuEntry("Solution circle", 1);
    glutAddMenuEntry("\"Useful solutions\"", 2);
    glutAddMenuEntry("Max distance", 3);
    glutAttachMenu(GLUT_MIDDLE_BUTTON);

    glutMotionFunc(on_move);
    glutPassiveMotionFunc(on_move);
    glutMouseFunc(on_button);
    glutKeyboardFunc(on_key_down);
    glutKeyboardUpFunc(on_key_up);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#if 0
    /* looks worse than without */
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
#endif

    glutDisplayFunc(on_frame);
    work(0);
    glutMainLoop();
    return 0;
}
