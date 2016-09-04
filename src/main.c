#include "method_stencil.h"
#include "method_segments.h"
#include "method_arcs.h"
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

#include <math.h>
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
struct ar_shader shaders[10];
GLuint vbo;
GLuint fbo;
int fbo_width = 64;
int fbo_height = 64;
struct ar_texture fbo_texture[1];
struct ar_texture fbo_stencil_texture[1];

/*
0: stencil buffer
1: fill between arcs
2: approximate with line segments
*/
int method = 0;

vec2 screen_to_world(vec2 screen_pos){
    return m23mulv2(m23inv(window.world_to_screen), screen_pos);
}

void ar_draw_points(struct ar_shader *shader, const vec2 *points, int n, uint32_t color, GLenum mode){
    struct ar_vertex *vertices = (struct ar_vertex*)malloc(sizeof(*vertices)*n);
    int i;
    for (i = 0; i < n; i++){
        vec2 p = points[i];
        vertices[i] = ar_vert(p.x, p.y, 0.0f, 0.0f, color);
    }
    ar_draw(shader, vertices, n, mode, vbo);
    free(vertices);
}

void ar_draw_line(struct ar_shader *shader, vec2 a, vec2 b, uint32_t color){
    struct ar_vertex vertices[2];
    vertices[0] = ar_vert(a.x, a.y, 0.0f, 0.0f, color);
    vertices[1] = ar_vert(b.x, b.y, 0.0f, 0.0f, color);
    ar_draw(shader, vertices, 2, GL_LINES, vbo);
}

void ar_bezier3_draw(struct ar_shader *shader, const struct ar_bezier3 *curve, uint32_t color){
    int n = 100;
    vec2 *points = (vec2*)malloc(sizeof*(points)*n);
    ar_bezier3_points(curve, points, n, 0.0, 1.0);
    ar_draw_points(shader, points, n, color, GL_LINE_STRIP);
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

void ar_draw_circle(struct ar_shader *shader, vec2 center, double radius, uint32_t color){
    int n = 100;
    vec2 *points = (vec2*)malloc(sizeof*(points)*n);
    ar_circle_points(points, n, center, radius);
    ar_draw_points(shader, points, n, color, GL_LINE_LOOP);
    free(points);
}

void ar_draw_arc(struct ar_shader *shader, struct ar_arc *arc, uint32_t color){
    int n = 100;
    vec2 *points = (vec2*)malloc(n*sizeof(*points));
    ar_arc_points(arc, points, n, 0.0, 1.0);
    ar_draw_points(shader, points, n, color, GL_LINE_STRIP);
}

void ar_draw_arrow(struct ar_shader *shader, vec2 a, vec2 b, double r, uint32_t color){
    vec2 d = v2scale(v2sub(b, a), r);

    vec2 points[3];
    points[0] = a;
    points[1] = b;
    ar_draw_points(shader, points, 2, color, GL_LINES);

    points[0] = b;
    points[1] = v2sub(b, v2add(v2smul(2.0, d), v2left(d)));
    points[2] = v2sub(b, v2sub(v2smul(2.0, d), v2left(d)));

    ar_draw_points(shader, points, 3, color, GL_TRIANGLES);
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

void bind_texture(struct ar_shader *shader, struct ar_texture *texture){
    if (shader->uniforms[1] != -1){
        glActiveTexture(GL_TEXTURE0);
        ar_texture_bind(texture);
        glUniform1i(shader->uniforms[1], 0);
    }
}

void init_fbo(void){
    /* initialize fbo texture */
    ar_texture_init(fbo_texture, fbo_width, fbo_height, NULL);
    //ar_texture_linear(fbo_texture);
    ar_texture_init_stencil(fbo_stencil_texture, fbo_width, fbo_height);

    /* initialize fbo */
    GL_CHECK
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture->id, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, fbo_stencil_texture->id, 0);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void draw_fbo(void){
    glViewport(0, 0, window.width, window.height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    ar_shader_use(&shaders[0]);
    GL_CHECK
    ar_upload_model_view_projection(&shaders[0], m4id());
    GL_CHECK
    bind_texture(&shaders[0], fbo_texture);
    //glGenerateMipmap(GL_TEXTURE_2D);
    GL_CHECK
    struct ar_vertex vertices[2*3];
    ar_make_rect(vertices, -1, -1, +1, +1, 0, 0, 1, 1, AR_WHITE);
    ar_draw(&shaders[0], vertices, 2*3, GL_TRIANGLES, vbo);
    GL_CHECK
}

void on_frame(void){
    static int pixels = 0;
    int pixels_increment = 256*256;

    pixels += pixels_increment;

    if (pixels > 4096*4096){
        pixels = pixels_increment;

        method++;
        if (method > 2){
            exit(0);
        }
    }

    fbo_width = fbo_height = sqrt(pixels);

    init_fbo();
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    struct ar_shader *shader = NULL;

    switch (method){
        case 0: shader = &shaders[1]; break;
        case 1: shader = &shaders[2]; break;
        case 2: shader = &shaders[3]; break;
    }

    switch (method){
        case 0: upload_svg(shader, vbo); break;
        case 1: upload_arcs(shader, vbo); break;
        case 2: upload_segments(shader, vbo); break;
    }

    GL_CHECK
    glViewport(0, 0, fbo_width, fbo_height);
    mat4 projection = m4_ortho2d(0.0f, 800, 800, 0.0f);
    mat23 world_to_screen = window.world_to_screen;
    mat4 modelview = m4m23(world_to_screen);
    mat4 mvp = m4mul(projection, modelview);

    GL_CHECK
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    GL_CHECK

    window.width = glutGet(GLUT_WINDOW_WIDTH);
    window.height = glutGet(GLUT_WINDOW_HEIGHT);

    ar_shader_use(shader);
    GL_CHECK

    ar_upload_model_view_projection(shader, mvp);
    GL_CHECK

    GL_CHECK
    GLuint timeElapsedQuery;

    glGenQueries(1, &timeElapsedQuery);
    GL_CHECK

    glBeginQuery(GL_TIME_ELAPSED, timeElapsedQuery);

    switch (method){
        case 0: render_svg(shader, vbo, mvp, projection); break;
        case 1: render_arcs(); break;
        case 2: render_segments(); break;
    }

    glEndQuery(GL_TIME_ELAPSED);

    draw_fbo();

    glutSwapBuffers();

    ar_texture_free(fbo_texture);
    ar_texture_free(fbo_stencil_texture);
    glDeleteFramebuffers(1, &fbo);

    GL_CHECK
    uint64_t t;
    glGetQueryObjectui64v(timeElapsedQuery, GL_QUERY_RESULT, &t);
    GL_CHECK
    printf("%d %d %d %f\n", method, fbo_width, fbo_height, t*1e-6);
    GL_CHECK
    glDeleteQueries(1, &timeElapsedQuery);
    GL_CHECK
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

const char *vert_src =
    "#version 120\r\n"
    AR_STR(
    attribute vec4 a_data0;
    attribute vec4 a_data1;
    attribute vec4 a_data2;
    attribute vec4 a_data3;

    varying vec4 v_data0;
    varying vec4 v_data1;
    varying vec4 v_data2;
    varying vec4 v_data3;

    uniform mat4 u_0;

    void main(){
        v_data0 = a_data0;
        v_data1 = a_data1;
        v_data2 = a_data2;
        v_data3 = a_data3;

        gl_Position = u_0*vec4(a_data0.xy, 0.0, 1.0);
    }
);

void make_texture_shader(struct ar_shader *shader){
    const char *frag_src =
        "#version 120\r\n"
        AR_STR(
        varying vec4 v_data0;
        varying vec4 v_data1;
        varying vec4 v_data2;
        varying vec4 v_data3;

        uniform sampler2D u_1;

        void main(){
            gl_FragColor = texture2D(u_1, v_data0.zw) * v_data1;
        }
    );

    ar_shader_init(shader, vert_src, frag_src);
}

void make_color_shader(struct ar_shader *shader){
    const char *frag_src =
        "#version 120\r\n"
        AR_STR(
        varying vec4 v_data0;
        varying vec4 v_data1;
        varying vec4 v_data2;
        varying vec4 v_data3;

        uniform sampler2D u_1;

        void main(){
            gl_FragColor = v_data1;
        }
    );

    ar_shader_init(shader, vert_src, frag_src);
}

void make_stencil_shader(struct ar_shader *shader){
    const char *frag_src =
        "#version 120\r\n"
        AR_STR(
        varying vec4 v_data0;
        varying vec4 v_data1;
        varying vec4 v_data2;
        varying vec4 v_data3;

        uniform sampler2D u_1;

        void main(){
            vec2 uv = v_data0.zw;
            vec4 color = v_data1;

            float r = dot(uv, uv);

            if (r > 1.0) discard;

            gl_FragColor = color;
        }
    );

    ar_shader_init(shader, vert_src, frag_src);
}

void make_arc_shader(struct ar_shader *shader){
    const char *frag_src =
        "#version 120\r\n"
        AR_STR(
        varying vec4 v_data0;
        varying vec4 v_data1;
        varying vec4 v_data2;
        varying vec4 v_data3;

        void main(){
            vec2 p = v_data0.xy;

            vec2 c_lower = v_data2.xy;
            vec2 c_upper = v_data2.zw;

            float r_lower = v_data3.x;
            float r_upper = v_data3.y;

            float type_lower = v_data3.z;
            float type_upper = v_data3.w;

            vec2 pa = p - c_lower;
            vec2 pb = p - c_upper;

            float ya = sqrt(r_lower*r_lower - pa.x*pa.x);
            float yb = sqrt(r_upper*r_upper - pb.x*pb.x);

            if (type_lower == 0.0) ya = -ya;
            if (type_upper == 0.0) yb = -yb;
            if (type_lower == 2.0) ya = pa.y;
            if (type_upper == 2.0) yb = pb.y;

            float alpha = ya <= pa.y && pb.y <= yb ? 1.0 : 0.0;

            vec4 color = v_data1;

            color.a *= alpha;

            gl_FragColor = color;
        }
    );

    ar_shader_init(shader, vert_src, frag_src);
}

int main(int argc, char **argv){
    window.width = 800;
    window.height = 800;
    window.world_to_screen = m23id();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);
    glutInitWindowSize(window.width, window.height);
    glutCreateWindow("");

    glewInit();

    make_texture_shader(&shaders[0]);
    make_stencil_shader(&shaders[1]);
    make_arc_shader(&shaders[2]);
    make_color_shader(&shaders[3]);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    int max_vertices = 1024*1024;
    glBufferData(GL_ARRAY_BUFFER, sizeof(struct ar_vertex)*max_vertices, NULL, GL_STATIC_DRAW);

    glutMotionFunc(on_move);
    glutPassiveMotionFunc(on_move);
    glutMouseFunc(on_button);
    glutKeyboardFunc(on_key_down);
    glutKeyboardUpFunc(on_key_up);
    glutDisplayFunc(on_frame);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    prepare_svg("development/py/beziers.svg");
    prepare_arcs_renderer("development/py/arcs.txt");
    prepare_segments("development/py/beziers.txt");

    work(0);
    glutMainLoop();
    return 0;
}
