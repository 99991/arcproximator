#include "out_of_sight.h"

#if 0
#define USE_STENCIL_BUFFER
#endif

void on_frame(void){
    GL_CHECK
    mat4 projection = m4_ortho2d(0.0f, window.width, window.height, 0.0f);
    mat23 world_to_screen = window.world_to_screen;
    mat4 modelview = m4m23(world_to_screen);
    mat4 mvp = m4mul(projection, modelview);

    GL_CHECK
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    GL_CHECK

    window.width = glutGet(GLUT_WINDOW_WIDTH);
    window.height = glutGet(GLUT_WINDOW_HEIGHT);

    ar_shader_use(arc_shader);
    GL_CHECK

    upload_model_view_projection(mvp);
    GL_CHECK

    if (utex0 != -1){
        glActiveTexture(GL_TEXTURE0);
        ar_texture_bind(texture);
        glUniform1i(utex0, 0);
    }


    GL_CHECK
    GLuint timeElapsedQuery;

    glGenQueries(1, &timeElapsedQuery);
    GL_CHECK

    glBeginQuery(GL_TIME_ELAPSED, timeElapsedQuery);

#ifdef USE_STENCIL_BUFFER
    render_svg(mvp, projection);
#else
    render_curves();
#endif
    glEndQuery(GL_TIME_ELAPSED);

    glutSwapBuffers();

    GL_CHECK
    uint64_t t;
    glGetQueryObjectui64v(timeElapsedQuery, GL_QUERY_RESULT, &t);
    GL_CHECK
    printf("Time Elapsed: %f milliseconds\n", t*1e-6);
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
#ifdef USE_STENCIL_BUFFER
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
        varying vec2 vtex;
        varying vec4 vcol;

        uniform sampler2D utex0;

        void main(){
            float r = dot(vtex, vtex);

            if (r > 1.0) discard;

            gl_FragColor = vcol;
            /*
            gl_FragColor = texture2D(utex0, vtex) * vcol;
            */
        }
    );
#else
    const char *vert_src =
        "#version 120\r\n"
        AR_STR(
        attribute vec4 apos;
        attribute vec2 atex;
        attribute vec4 acol;
        attribute vec4 a_data0;
        attribute vec4 a_data1;

        varying vec2 vpos;
        varying vec2 vtex;
        varying vec4 vcol;

        varying vec4 v_data0;
        varying vec4 v_data1;

        uniform mat4 umvp;

        void main(){
            v_data0 = a_data0;
            v_data1 = a_data1;
            vtex = atex;
            vcol = acol;
            vpos = apos.xy;
            vec4 pos = umvp*vec4(apos.xyz, 1.0);
            gl_Position = pos;
        }
    );

    const char *frag_src =
        "#version 120\r\n"
        AR_STR(
        varying vec2 vpos;
        varying vec2 vtex;
        varying vec4 vcol;

        varying vec4 v_data0;
        varying vec4 v_data1;

        uniform sampler2D utex0;

        void main(){
            vec2 p = vpos;

            vec2 c_lower = v_data0.xy;
            vec2 c_upper = v_data0.zw;

            float r_lower = v_data1.x;
            float r_upper = v_data1.y;

            float type_lower = v_data1.z;
            float type_upper = v_data1.w;

            vec2 pa = p - c_lower;
            vec2 pb = p - c_upper;

            float ya = sqrt(r_lower*r_lower - pa.x*pa.x);
            float yb = sqrt(r_upper*r_upper - pb.x*pb.x);

            if (type_lower == 0.0) ya = -ya;
            if (type_upper == 0.0) yb = -yb;
            if (type_lower == 2.0) ya = pa.y;
            if (type_upper == 2.0) yb = pb.y;

            float alpha = ya <= pa.y && pb.y <= yb ? 1.0 : 0.0;

            vec4 color = vcol;

            color.a *= alpha;

            gl_FragColor = color;
/*
            vec4 color = texture2D(utex0, vtex) * vcol;

            color.a *= alpha;

            gl_FragColor = color;
            */
        }
    );
#endif

    int x, y;
    int nx = 16;
    int ny = 16;
    uint32_t texture_data[16*16];
    for (y = 0; y < ny; y++) for (x = 0; x < nx; x++){
        texture_data[x + y*nx] =  0xffffffff;
    }

    window.width = 800;
    window.height = 800;
    window.world_to_screen = m23id();

    control_points[0] = v2(200, 100);
    control_points[1] = v2(200, 200);
    control_points[2] = v2(200, 200);
    control_points[3] = v2(100, 200);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);
    glutInitWindowSize(window.width, window.height);
    glutCreateWindow("");

    glewInit();

    ar_shader_init(arc_shader, vert_src, frag_src);

    umvp  = glGetUniformLocation(arc_shader->program, "umvp");
    utex0 = glGetUniformLocation(arc_shader->program, "utex0");
    apos  = glGetAttribLocation(arc_shader->program, "apos");
    atex  = glGetAttribLocation(arc_shader->program, "atex");
    acol  = glGetAttribLocation(arc_shader->program, "acol");

#ifdef USE_STENCIL_BUFFER
#else
    a_data0 = glGetAttribLocation(arc_shader->program, "a_data0");
    a_data1 = glGetAttribLocation(arc_shader->program, "a_data1");
#endif

    ar_texture_init(texture, nx, ny, texture_data);

    glGenBuffers(1, &vbos[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(struct ar_vertex)*MAX_VERTICES, NULL, GL_STATIC_DRAW);
    glGenBuffers(1, &vbos[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(struct ar_vertex)*MAX_VERTICES, NULL, GL_STATIC_DRAW);

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

#ifdef USE_STENCIL_BUFFER
    prepare_svg("development/py/beziers.svg");
#else
    prepare_curve_renderer("development/py/arcs.txt");
#endif

    glutDisplayFunc(on_frame);
    work(0);
    glutMainLoop();
    return 0;
}
