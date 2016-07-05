#include "my_arc.h"
#include "my_bezier3.h"
#include "color.h"
#include "durand_kerner.h"

#include <assert.h>
#include <stdio.h>

struct my_window {
    int width;
    int height;
    int is_mouse_button_down[256];
    int is_key_down[256];
    vec2 mouse_pos;
    vec2 mouse_pos_old;
    mat23 world_to_screen;
};

struct my_window window;

vec2 screen_to_world(vec2 screen_pos){

    mat23 world_to_screen_transform = window.world_to_screen;
    mat23 screen_to_world_transform = m23inv(world_to_screen_transform);

    vec2 world_pos = m23mulv2(screen_to_world_transform, screen_pos);

    return world_pos;
}

struct bezier_dist {
    vec2 p;
    vec2 q;
    double t;
    double dist2;
};

struct bezier_dist get_closest(struct bezier_dist a, struct bezier_dist b){
    return a.dist2 < b.dist2 ? a : b;
}

struct bezier_dist get_furthest(struct bezier_dist a, struct bezier_dist b){
    return a.dist2 > b.dist2 ? a : b;
}

int my_bezier3_dist_roots(
    const struct my_bezier3 *curve,
    vec2 point,
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
    double x = point.x;
    double y = point.y;

    ASSERT_NOT_NAN(ax);
    ASSERT_NOT_NAN(bx);
    ASSERT_NOT_NAN(cx);
    ASSERT_NOT_NAN(dx);
    ASSERT_NOT_NAN(x);
    ASSERT_NOT_NAN(ay);
    ASSERT_NOT_NAN(by);
    ASSERT_NOT_NAN(cy);
    ASSERT_NOT_NAN(dy);
    ASSERT_NOT_NAN(y);

    double coeffs[6] = {
        6*(ax*ax) - 36*ax*bx + 36*ax*cx - 12*ax*dx + 6*(ay*ay) - 36*ay*by + 36*ay*cy - 12*ay*dy + 54*(bx*bx) - 108*bx*cx + 36*bx*dx + 54*(by*by) - 108*by*cy + 36*by*dy + 54*(cx*cx) - 36*cx*dx + 54*(cy*cy) - 36*cy*dy + 6*(dx*dx) + 6*(dy*dy),
        -30*(ax*ax) + 150*ax*bx - 120*ax*cx + 30*ax*dx - 30*(ay*ay) + 150*ay*by - 120*ay*cy + 30*ay*dy - 180*(bx*bx) + 270*bx*cx - 60*bx*dx - 180*(by*by) + 270*by*cy - 60*by*dy - 90*(cx*cx) + 30*cx*dx - 90*(cy*cy) + 30*cy*dy,
        60*(ax*ax) - 240*ax*bx + 144*ax*cx - 24*ax*dx + 60*(ay*ay) - 240*ay*by + 144*ay*cy - 24*ay*dy + 216*(bx*bx) - 216*bx*cx + 24*bx*dx + 216*(by*by) - 216*by*cy + 24*by*dy + 36*(cx*cx) + 36*(cy*cy),
        -60*(ax*ax) + 180*ax*bx - 72*ax*cx + 6*ax*dx + 6*ax*x - 60*(ay*ay) + 180*ay*by - 72*ay*cy + 6*ay*dy + 6*ay*y - 108*(bx*bx) + 54*bx*cx - 18*bx*x - 108*(by*by) + 54*by*cy - 18*by*y + 18*cx*x + 18*cy*y - 6*dx*x - 6*dy*y,
        30*(ax*ax) - 60*ax*bx + 12*ax*cx - 12*ax*x + 30*(ay*ay) - 60*ay*by + 12*ay*cy - 12*ay*y + 18*(bx*bx) + 24*bx*x + 18*(by*by) + 24*by*y - 12*cx*x - 12*cy*y,
        -6*(ax*ax) + 6*ax*bx + 6*ax*x - 6*(ay*ay) + 6*ay*by + 6*ay*y - 6*bx*x - 6*by*y,
    };

    int iterations = durand_kerner(coeffs, 6, roots, 1000, 1e-15);

    if (iterations < 0){
        printf("ERROR: durand kerner method failed to converge\n");
        /* TODO remove this */
        exit(-1);
        return iterations;
    }

    int i;
    for (i = 0; i < 5; i++){
        vec2 root = roots[i];
        ASSERT_NOT_NAN(root.x);
        ASSERT_NOT_NAN(root.y);
        vec2 p = polynomial_eval_complex(coeffs, 6, roots[i]);
        if (v2len(p) > 1e-5){
            printf("ERROR: root %e %e is not zero enough: %e %e\n", root.x, root.y, p.x, p.y);
        }
    }

    return iterations;
}

vec2 p[4];

void arc_from_points_and_normal(struct my_arc *arc, vec2 start, vec2 end, vec2 start_normal, int clockwise){
    vec2 v = v2sub(end, start);
    double radius = v2dot(v, v)*0.5/v2dot(v, start_normal);
    ASSERT_NOT_NAN(radius);
    vec2 center = v2add(start, v2smul(radius, start_normal));

    my_arc_init(arc, center, radius, start, end, clockwise);
}

struct bezier_dist my_bezier3_arcs_max_dist_approx(
    const struct my_bezier3 *curve,
    const struct my_arc *arcs
){
    struct bezier_dist max_dist;
    max_dist.p = curve->control_points[0];
    max_dist.q = max_dist.p;
    max_dist.t = 0.0;
    max_dist.dist2 = -DBL_INF;

    int n = 10*1000;
    int i;
    for (i = 0; i <= n; i++){
        double t = i*(1.0/n);
        vec2 p = my_bezier3_at(curve, t);

        struct bezier_dist a, b;
        a.p = p;
        b.p = p;
        a.q = my_arc_clamp(arcs + 0, p);
        b.q = my_arc_clamp(arcs + 1, p);
        a.t = t;
        b.t = t;
        a.dist2 = v2dist2(a.p, a.q);
        b.dist2 = v2dist2(a.p, b.q);

        max_dist = get_furthest(max_dist, get_closest(a, b));
    }

    return max_dist;
}

struct bezier_dist my_bezier3_arcs_max_dist(struct my_bezier3 *curve, struct my_arc *arcs){
    struct bezier_dist max_dist;
    max_dist.p = curve->control_points[0];
    max_dist.t = 0.0;
    max_dist.dist2 = 0.0;

    /* TODO check iterations */
    vec2 roots[30];
    my_bezier3_dist_roots(curve, arcs[0].center, roots + 0*5);
    my_bezier3_dist_roots(curve, arcs[1].center, roots + 1*5);
    my_bezier3_dist_roots(curve, arcs[0].start , roots + 2*5);
    my_bezier3_dist_roots(curve, arcs[0].end   , roots + 3*5);
    my_bezier3_dist_roots(curve, arcs[1].start , roots + 4*5);
    my_bezier3_dist_roots(curve, arcs[1].end   , roots + 5*5);

    int i;
    for (i = 0; i < 30; i++){
        vec2 root = roots[i];
        double t = root.x;

        if (t < 0 || t > 1) continue;
        /* TODO check how real this is */
        if (fabs(root.y) > 0.01) continue;

        vec2 p = my_bezier3_at(curve, t);

        struct bezier_dist a, b;
        a.p = p;
        b.p = p;
        a.q = my_arc_clamp(arcs + 0, p);
        b.q = my_arc_clamp(arcs + 1, p);
        a.t = t;
        b.t = t;
        a.dist2 = v2dist2(a.p, a.q);
        b.dist2 = v2dist2(a.p, b.q);

        max_dist = get_furthest(max_dist, get_closest(a, b));
    }

    return max_dist;
}

int show_solution_circle  = 1;
int show_useful_solutions = 1;
int show_max_dist         = 0;
int show_control_points   = 1;
int show_biarc            = 1;
int show_convex_hull      = 1;

void menu_callback(int option){
    switch (option){
        case 1: show_solution_circle  = !show_solution_circle;  break;
        case 2: show_useful_solutions = !show_useful_solutions; break;
        case 3: show_max_dist         = !show_max_dist;         break;
        case 4: show_control_points   = !show_control_points;   break;
        case 5: show_biarc            = !show_biarc;            break;
        case 6: show_convex_hull      = !show_convex_hull;      break;
    }
}

int convex_hull4(const vec2 *p, vec2 *q){
    vec2 a = p[0];
    vec2 b = p[1];
    vec2 c = p[2];
    vec2 d = p[3];

    /* check if triangle is clockwise */
    if (v2det(v2sub(a, b), v2sub(a, c)) < 0.0){
        /* make it counterclockwise */
        MY_SWAP(vec2, a, c);
    }

    int dab = v2isleft(d, a, b);
    int dbc = v2isleft(d, b, c);
    int dca = v2isleft(d, c, a);

    /* point d is in triangle(a, b, c) */
    if ( dab &&  dbc &&  dca){ q[0] = a; q[1] = b; q[2] = c;           return 3; }
    if ( dab &&  dbc && !dca){ q[0] = a; q[1] = b; q[2] = c; q[3] = d; return 4; }
    if ( dab && !dbc &&  dca){ q[0] = a; q[1] = b; q[2] = d; q[3] = c; return 4; }
    if ( dab && !dbc && !dca){ q[0] = a; q[1] = b; q[2] = d;           return 3; }
    if (!dab &&  dbc &&  dca){ q[0] = a; q[1] = d; q[2] = b; q[3] = c; return 4; }
    if (!dab &&  dbc && !dca){ q[0] = d; q[1] = b; q[2] = c;           return 3; }
    if (!dab && !dbc &&  dca){ q[0] = a; q[1] = d; q[2] = c;           return 3; }

    puts("ERROR: d is left of all three sides of a counterclockwise triangle");

    return 0;
}

void approximate_curve_with_arcs(){
    struct my_bezier3 curve[1];
    my_bezier3_init(curve, p[0], p[1], p[2], p[3]);

    vec2 a = p[0];
    vec2 b = p[3];
    vec2 tangent_a = v2normalize(v2sub(p[1], a));
    vec2 tangent_b = v2normalize(v2sub(p[2], b));
    ASSERT_NOT_NAN(tangent_a.x);
    ASSERT_NOT_NAN(tangent_a.y);
    ASSERT_NOT_NAN(tangent_b.x);
    ASSERT_NOT_NAN(tangent_b.y);

    /* rotation matrix R = {{c, -s}, {s, c}} so that: R*tangent_a = tangent_b */
    double c = v2dot(tangent_a, tangent_b);
    double s = v2det(tangent_a, tangent_b);

    /* find center of rotation so that: R*(a - center) + center = b */
    vec2 center;
    center.x = (a.x + b.x - s*(a.y - b.y)/(1.0 + c))*0.5;
    center.y = (a.y + b.y + s*(a.x - b.x)/(1.0 + c))*0.5;
    ASSERT_NOT_NAN(center.x);
    ASSERT_NOT_NAN(center.y);

    double radius = v2dist(a, center);

    vec2 world_mouse_pos = screen_to_world(window.mouse_pos);
    vec2 join = v2add(center, v2scale(v2add(v2sub(world_mouse_pos, center), v2(1, 1)), radius));
    ASSERT_NOT_NAN(join.x);
    ASSERT_NOT_NAN(join.y);

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
    ASSERT_NOT_NAN(normal_a.x);
    ASSERT_NOT_NAN(normal_a.y);
    ASSERT_NOT_NAN(normal_b.x);
    ASSERT_NOT_NAN(normal_b.y);

    struct my_arc arcs[2];
    arc_from_points_and_normal(arcs + 0, a, join, normal_a, a_clockwise);
    arc_from_points_and_normal(arcs + 1, b, join, normal_b, b_clockwise);

    struct bezier_dist dist0 = my_bezier3_arcs_max_dist(curve, arcs);
    struct bezier_dist dist1 = my_bezier3_arcs_max_dist_approx(curve, arcs);

    if (show_max_dist){
        printf("dist: %f %f %f, %f %f\n\n", sqrt(dist0.dist2) - sqrt(dist1.dist2), sqrt(dist0.dist2), sqrt(dist1.dist2), dist0.t, dist1.t);

        SET_COLOR(WHITE);
        draw_line(dist0.p, dist0.q);
        draw_circle(dist0.p, 3.0);
        draw_circle(dist0.q, 3.0);

        SET_COLOR(LIGHT_BLUE);
        draw_line(dist1.p, dist1.q);
        draw_circle(dist1.p, 5.0);
        draw_circle(dist1.q, 5.0);
    }

    if (show_solution_circle){
        SET_COLOR(YELLOW);
        draw_circle(center, radius);
    }

    if (show_control_points){
        SET_COLOR(DARK_GRAY);
        draw(p, 4, GL_LINE_STRIP);

        SET_COLOR(GREEN);
        draw_arrow(a, p[1], 5.0);

        SET_COLOR(RED);
        draw_arrow(b, p[2], 5.0);
    }

    if (show_convex_hull){
        vec2 q[4];
        int n = convex_hull4(p, q);
        glColor4f(1, 1, 1, 0.1f);
        draw(q, n, GL_TRIANGLE_FAN);
    }

    SET_COLOR(WHITE);
    my_bezier3_draw(curve);

    if (show_biarc){
        SET_COLOR(YELLOW);
        draw_circle(join, 5.0);

        SET_COLOR(GREEN);
        my_arc_draw(arcs + 0);

        SET_COLOR(RED);
        my_arc_draw(arcs + 1);
    }

    if (show_useful_solutions){
        int aright = v2isright(a, p[1], p[2]);
        int bright = v2isright(b, p[1], p[2]);
        int positive = s > 0.0;
        int clockwise = aright != bright ? /* S */ positive : /* U */ aright;

        struct my_arc arc[1];
        my_arc_init(arc, center, radius, a, b, clockwise);
        SET_COLOR(LIGHT_BLUE);
        my_arc_draw(arc);
    }
}

void on_frame(){
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    window.width = glutGet(GLUT_WINDOW_WIDTH);
    window.height = glutGet(GLUT_WINDOW_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, window.width, 0, window.height, -1, +1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    mat23 world_to_screen = window.world_to_screen;

    mat4 modelview = m4m23(world_to_screen);
    glMultMatrixd(modelview.data);

    approximate_curve_with_arcs();

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
            if (!closest || v2dist2(p[i], world_mouse_pos) < v2dist2(*closest, world_mouse_pos)){
                closest = &p[i];
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
    UNUSED(x);
    UNUSED(y);

    window.is_key_down[key] = 1;

    if (key == 'q') exit(0);
}

void on_key_up(unsigned char key, int x, int y){
    UNUSED(x);
    UNUSED(y);

    window.is_key_down[key] = 0;
}

int main(int argc, char **argv){
    window.width = 512;
    window.height = 512;
    window.world_to_screen = m23id();

    p[0] = v2(100, 100);
    p[1] = v2(500, 150);
    p[2] = v2(50, 300);
    p[3] = v2(400, 500);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(window.width, window.height);
    glutCreateWindow("");

    glutCreateMenu(menu_callback);
    glutAddMenuEntry("Control points", 4);
    glutAddMenuEntry("Biarc", 5);
    glutAddMenuEntry("Solution circle", 1);
    glutAddMenuEntry("\"Useful solutions\"", 2);
    glutAddMenuEntry("Max distance", 3);
    glutAddMenuEntry("Convex hull", 6);
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
