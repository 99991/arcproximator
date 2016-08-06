#include "out_of_sight.h"

void on_frame(void){
    mat4 projection = m4_ortho2d(0.0f, window.width, window.height, 0.0f);
    mat23 world_to_screen = window.world_to_screen;
    mat4 modelview = m4m23(world_to_screen);
    mat4 mvp = m4mul(projection, modelview);

    gl_CHECK
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    gl_CHECK

    window.width = glutGet(GLUT_WINDOW_WIDTH);
    window.height = glutGet(GLUT_WINDOW_HEIGHT);

    ar_shader_use(arc_shader);
    gl_CHECK

    upload_model_view_projection(mvp);
    gl_CHECK

    glActiveTexture(GL_TEXTURE0);
    ar_texture_bind(texture);
    glUniform1i(utex0, 0);

#if 1
    draw_svg(mvp, projection);
#else

#endif

    glutSwapBuffers();
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

    glewInit();

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
