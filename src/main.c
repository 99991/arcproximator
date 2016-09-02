#include "out_of_sight.h"

#if 1
#define USE_STENCIL_BUFFER
#endif

void bind_texture(struct ar_shader *shader, struct ar_texture *texture){
    if (shader->uniforms[1] != -1){
        glActiveTexture(GL_TEXTURE0);
        ar_texture_bind(texture);
        glUniform1i(shader->uniforms[1], 0);
    }
}

void ar_texture_init_stencil(struct ar_texture *texture, int width, int height){
    texture->width = width;
    texture->height = height;
    glGenTextures(1, &texture->id);

    ar_texture_bind(texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
}

void init_fbo(void){
    /* initialize fbo texture */
    ar_texture_init(fbo_texture, fbo_width, fbo_height, NULL);
    ar_texture_linear(fbo_texture);
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
    upload_model_view_projection(&shaders[0], m4id());
    GL_CHECK
    bind_texture(&shaders[0], fbo_texture);
    glGenerateMipmap(GL_TEXTURE_2D);
    GL_CHECK
    struct ar_vertex vertices[2*3];
    ar_make_rect(vertices, -1, -1, +1, +1, 0, 0, 1, 1, AR_WHITE);
    ar_draw(&shaders[0], vertices, 2*3, GL_TRIANGLES, vbo);
    GL_CHECK
}

void on_frame(void){
    init_fbo();
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

#ifdef USE_STENCIL_BUFFER
    upload_svg();
#else
    upload_curves();
#endif

    GL_CHECK
    //mat4 projection = m4_ortho2d(0.0f, window.width, window.height, 0.0f);
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

#ifdef USE_STENCIL_BUFFER
    struct ar_shader *shader = &shaders[1];
#else
    struct ar_shader *shader = &shaders[2];
#endif
    ar_shader_use(shader);
    GL_CHECK

    upload_model_view_projection(shader, mvp);
    GL_CHECK

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

    draw_fbo();

    glutSwapBuffers();

    ar_texture_free(fbo_texture);
    ar_texture_free(fbo_stencil_texture);
    glDeleteFramebuffers(1, &fbo);

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
            //gl_FragColor = texture2D(u_1, uv) * color;
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

        uniform sampler2D u_1;

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

            //vec4 color = texture2D(u_1, v_data0.zw) * v_data1;
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

    control_points[0] = v2(200, 100);
    control_points[1] = v2(200, 200);
    control_points[2] = v2(200, 200);
    control_points[3] = v2(100, 200);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);
    glutInitWindowSize(window.width, window.height);
    glutCreateWindow("");

    glewInit();

    make_texture_shader(&shaders[0]);
    make_stencil_shader(&shaders[1]);
    make_arc_shader(&shaders[2]);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
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
    glutDisplayFunc(on_frame);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef USE_STENCIL_BUFFER
    prepare_svg("development/py/beziers.svg");
#else
    prepare_curve_renderer("development/py/arcs.txt");
#endif

    work(0);
    glutMainLoop();
    return 0;
}
