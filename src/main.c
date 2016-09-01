#include "out_of_sight.h"

#if 0
#define USE_STENCIL_BUFFER
#endif

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

#ifdef USE_STENCIL_BUFFER
    draw_svg(mvp, projection);
#else

    int i, n;
    FILE *fp = fopen("development/py/arcs.txt", "rb");
    assert(fp);
    fscanf(fp, "%d", &n);
    printf("%i arcs\n", n);

    assert(n % 2 == 0);

    int n_vertices = n/2*6;

    struct ar_arc *arcs = malloc(sizeof(*arcs)*n);
    struct ar_vertex *vertices = malloc(sizeof(*vertices)*n_vertices);

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

        struct ar_vertex v;
        v.color = AR_WHITE;
        v.u = 0.0f;
        v.v = 0.0f;

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

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(*vertices)*n_vertices, vertices);

    if (apos != -1){
        glEnableVertexAttribArray(apos);
        glVertexAttribPointer(apos, 2, GL_FLOAT, GL_FALSE, sizeof(*vertices), (char*)0);
    }

    if (atex != -1){
        glEnableVertexAttribArray(atex);
        glVertexAttribPointer(atex, 2, GL_FLOAT, GL_FALSE, sizeof(*vertices), (char*)8);
    }

    if (acol != -1){
        glEnableVertexAttribArray(acol);
        glVertexAttribPointer(acol, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(*vertices), (char*)16);
    }

    if (a_data0 != -1){
        glEnableVertexAttribArray(a_data0);
        glVertexAttribPointer(a_data0, 4, GL_FLOAT, GL_FALSE, sizeof(*vertices), (char*)20);
    }

    if (a_data1 != -1){
        glEnableVertexAttribArray(a_data1);
        glVertexAttribPointer(a_data1, 4, GL_FLOAT, GL_FALSE, sizeof(*vertices), (char*)36);
    }

    GLuint timeElapsedQuery;

    glGenQueries(1, &timeElapsedQuery);
    glBeginQuery(GL_TIME_ELAPSED, timeElapsedQuery);

    glDrawArrays(GL_TRIANGLES, 0, vertex_ptr - vertices);
    /*
    ar_draw(vertices, vertex_ptr - vertices, GL_TRIANGLES, apos, atex, acol);
    */
    glEndQuery(GL_TIME_ELAPSED);

    free(vertices);
#endif

    glutSwapBuffers();

    uint64_t t;
    glGetQueryObjectui64v(timeElapsedQuery, GL_QUERY_RESULT, &t);
    printf("Time Elapsed: %f milliseconds\n", t*1e-6);
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

            gl_FragColor = vec4(1.0);
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

            gl_FragColor = vec4(1.0, 1.0, 1.0, alpha);
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glutDisplayFunc(on_frame);
    work(0);
    glutMainLoop();
    return 0;
}
