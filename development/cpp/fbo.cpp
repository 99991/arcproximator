#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <vector>
#include <stdlib.h>
#include "MaxRectsBinPack.h"

#define CHECK_GL check_error(__LINE__);

#define STR(x) #x

#define PI_F 3.14159265358979f

#define MAX_ATTRIBUTES 5
#define MAX_UNIFORMS 5

#define MAX_SHAPES 20000

float box_outer_radius = 5.0f;
float box_inner_radius = 4.0f;
float box_grow_radius  = 2.0f;
float delta_radius     = 3.0f;
float inner_radius     = 50.0f;

int screen_width = 1024;
int screen_height = 1024;

int fbo_width = 2048;
int fbo_height = 2048;

bool check_shader_compile_status(GLuint obj) {
    GLint status;
    glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE) {
        GLint length;
        glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetShaderInfoLog(obj, length, &length, &log[0]);
        puts(log.data());
        return false;
    }
    return true;
}

bool check_program_link_status(GLuint obj) {
    GLint status;
    glGetProgramiv(obj, GL_LINK_STATUS, &status);
    if(status == GL_FALSE) {
        GLint length;
        glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetProgramInfoLog(obj, length, &length, &log[0]);
        puts(log.data());
        return false;
    }
    return true;
}

GLuint make_shader(const char *source, GLenum shader_type){
    GLuint shader = glCreateShader(shader_type);
    int length = strlen(source);
    glShaderSource(shader, 1, &source, &length);
    glCompileShader(shader);
    check_shader_compile_status(shader);
    return shader;
}

GLuint make_shader_program(const char *vert_src, const char *frag_src){
    GLuint program = glCreateProgram();

    GLuint vert_shader = make_shader(vert_src, GL_VERTEX_SHADER);
    GLuint frag_shader = make_shader(frag_src, GL_FRAGMENT_SHADER);

    glAttachShader(program, vert_shader);
    glAttachShader(program, frag_shader);

    glLinkProgram(program);

    check_program_link_status(program);

    return program;
}

struct Shader {
    GLuint program;
    GLuint attributes[MAX_ATTRIBUTES];
    GLuint uniforms[MAX_UNIFORMS];

    Shader(const char *vert_src, const char *frag_src){
        program = make_shader_program(vert_src, frag_src);

        use();

        for (int i = 0; i < MAX_UNIFORMS; i++){
            char name[4] = "u_0";
            name[2] = '0' + i;
            uniforms[i] = glGetUniformLocation(program, name);
            //printf("uniform location %i: %i\n", i, uniforms[i]);
        }
        for (int i = 0; i < MAX_ATTRIBUTES; i++){
            char name[8] = "a_data0";
            name[6] = '0' + i;
            attributes[i] = glGetAttribLocation(program, name);
            //printf("attribute location %i: %i\n", i, attributes[i]);
        }
    }

    void use(){
        glUseProgram(program);
    }
};

struct Vertex {
    float x, y, u, v;
    GLuint color;
};

typedef std::vector<Vertex> Vertices;

GLuint fbo;
GLuint color_texture;
GLuint stencil_texture;
GLuint depth_texture;
GLuint timeElapsedQuery;
GLuint vbo_immediate;
GLuint vbo_cover_fbo;
GLuint vbo_from_fbo_to_screen;
GLuint vbo_direct;
Vertices immediate_vertices;
Vertices cover_fbo_vertices;
Vertices from_fbo_to_screen_vertices;
Vertices direct_vertices;
Shader *color_shader;
Shader *texture_shader;

void check_error(int line){
    GLenum error = glGetError();
    if (error != GL_NO_ERROR){
        printf("ERROR in line %d: %s\n", line, (const char*)gluErrorString(error));
    }
}

GLuint make_texture(int width, int height){
    GLuint texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

#if 1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    return texture;
}

GLuint make_stencil_texture(int width, int height){
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

    return texture;
}

void make_fbo(void){
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, stencil_texture, 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

struct BoundingBox {
    float x0, y0, x1, y1;

    void enclose(float x, float y){
        x0 = std::min(x0, x);
        y0 = std::min(y0, y);
        x1 = std::max(x1, x);
        y1 = std::max(y1, y);
    }
};

void make_rect(
    Vertices &vertices,
    float x0, float y0,
    float x1, float y1,
    float u0 = 0.0f, float v0 = 0.0f,
    float u1 = 1.0f, float v1 = 1.0f,
    GLuint color = 0xffffffff
){
    Vertex v[4] = {
        Vertex{x0, y0, u0, v0, color},
        Vertex{x1, y0, u1, v0, color},
        Vertex{x1, y1, u1, v1, color},
        Vertex{x0, y1, u0, v1, color},
    };
    vertices.push_back(v[0]);
    vertices.push_back(v[1]);
    vertices.push_back(v[2]);

    vertices.push_back(v[0]);
    vertices.push_back(v[2]);
    vertices.push_back(v[3]);
}

struct Shape {
    Vertices vertices;
    BoundingBox boundingBox;
    rbp::Rect rect;
    rbp::Rect fbo_rect;
    uint32_t color;

    void update(){
        boundingBox.x0 = boundingBox.x1 = vertices[0].x;
        boundingBox.y0 = boundingBox.y1 = vertices[0].y;
        for (const Vertex &vertex : vertices){
            boundingBox.enclose(vertex.x, vertex.y);
        }
        rect.x = floor(boundingBox.x0);
        rect.y = floor(boundingBox.y0);
        rect.width = ceil(boundingBox.x1) - rect.x;
        rect.height = ceil(boundingBox.y1) - rect.y;
    }
};

void init_fbo(Shader *shader, int width, int height, GLuint fbo){
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    float m[16] = {
        2.0f/width, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f/height, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 1.0f,
    };

    shader->use();
    glUniformMatrix4fv(shader->uniforms[0], 1, GL_FALSE, m);
}

void set_attributes(Shader *shader){
    if (shader->attributes[0] != (GLuint)-1){
        glEnableVertexAttribArray(shader->attributes[0]);
        glVertexAttribPointer(shader->attributes[0], 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    }
    if (shader->attributes[1] != (GLuint)-1){
        glEnableVertexAttribArray(shader->attributes[1]);
        glVertexAttribPointer(shader->attributes[1], 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)16);
    }
}

void bind_texture(Shader *shader, GLuint texture){
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    shader->use();
    glUniform1i(shader->uniforms[1], 0);
}

void begin_stencil(){
    glEnable(GL_STENCIL_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glStencilFunc(GL_NEVER, 0, 1);
    glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);
}

void begin_stencil_covering(){
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilFunc(GL_EQUAL, 1, 1);
    glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
}

void end_stencil(){
    glDisable(GL_STENCIL_TEST);
}

double triangle_area(Vertex a, Vertex b, Vertex c){
    double bax = b.x - a.x;
    double bay = b.y - a.y;
    double cax = c.x - a.x;
    double cay = c.y - a.y;
    double area = 0.5*(bax*cay - bay*cax);
    return area < 0.0 ? -area : area;
}

double calculate_area(const Vertices &vertices){
    double area = 0.0;
    for (size_t i = 0; i < vertices.size(); i += 3){
        area += triangle_area(vertices[i], vertices[i+1], vertices[i+2]);
    }
    return area;
}

void draw_with_immediate_buffer(int n){
    init_fbo(color_shader, fbo_width, fbo_height, fbo);

    begin_stencil();

    // stencil
    glBindBuffer(GL_ARRAY_BUFFER, vbo_immediate);
    set_attributes(color_shader);
    glDrawArrays(GL_TRIANGLES, 0, immediate_vertices.size()/MAX_SHAPES*n);

    begin_stencil_covering();

    // cover
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cover_fbo);
    set_attributes(color_shader);
    glDrawArrays(GL_TRIANGLES, 0, cover_fbo_vertices.size()/MAX_SHAPES*n);

    end_stencil();

    // prepare screen
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    init_fbo(texture_shader, screen_width, screen_height, 0);
    bind_texture(texture_shader, color_texture);

#if 0
    // to dump fbo content to screen
    Vertices vertices;
    make_rect(vertices, 0.0f, 0.0f, screen_width, screen_height);
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    set_attributes(texture_shader);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glDeleteBuffers(1, &vbo);
#else
    // from fbo to screen
    glBindBuffer(GL_ARRAY_BUFFER, vbo_from_fbo_to_screen);
    set_attributes(texture_shader);
    glDrawArrays(GL_TRIANGLES, 0, from_fbo_to_screen_vertices.size()/MAX_SHAPES*n);
#endif
}

void draw_directly(int n){
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    init_fbo(color_shader, screen_width, screen_height, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_direct);
    set_attributes(color_shader);

    int vertices_per_shape = direct_vertices.size()/MAX_SHAPES;

    for (int i = 0; i < n; i++){
        begin_stencil();
        glDrawArrays(GL_TRIANGLES, vertices_per_shape*i, vertices_per_shape);
        begin_stencil_covering();
        glDrawArrays(GL_TRIANGLES, vertices_per_shape*i, vertices_per_shape);
        end_stencil();
    }
}

double sec(){
    LARGE_INTEGER t, frequency;
    QueryPerformanceCounter(&t);
    QueryPerformanceFrequency(&frequency);
    return t.QuadPart / (double)frequency.QuadPart;
}

void display(void){
    double t = sec();

    static int n = 0;
    static int method = 0;
    int increment = 1;

    n += increment;
    if (n > MAX_SHAPES){
        method++;
        n = increment;
        if (method >= 2){
            exit(0);
        }
    }

    glClearStencil(0);
    glClearColor(0.0f, 0.5f, 0.0f, 0.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBeginQuery(GL_TIME_ELAPSED, timeElapsedQuery);
    switch (method){
        case 0: draw_with_immediate_buffer(n); break;
        case 1: draw_directly(n); break;
    }
    glEndQuery(GL_TIME_ELAPSED);

    glFinish();
    glFlush();

    glutSwapBuffers();

    GLint available = 0;
    while (!available){
        glGetQueryObjectiv(timeElapsedQuery, GL_QUERY_RESULT_AVAILABLE, &available);
    }

    double dt = sec() - t;
    uint64_t nsec;
    glGetQueryObjectui64v(timeElapsedQuery, GL_QUERY_RESULT, &nsec);
    printf("%i %f %f\n", n, nsec*1e-6, dt*1000.0);

    CHECK_GL
}

uint32_t rgba_bytes(uint32_t r, uint32_t g, uint32_t b, uint32_t a){
    return (a << 24) | (b << 16) | (g << 8) | r;
}

uint32_t rgba(float r, float g, float b, float a){
    return rgba_bytes(r*255.0f, g*255.0f, b*255.0f, a*255.0f);
}

void init_vertices(){
    int i, n = MAX_SHAPES;
    std::vector<Shape> shapes;

    std::vector<uint32_t> nice_colors = {
        rgba(1.0f, 0.0f, 0.0f, 1.0f),
        rgba(0.0f, 1.0f, 0.0f, 1.0f),
        rgba(0.0f, 0.0f, 1.0f, 1.0f),
        rgba(0.0f, 1.0f, 1.0f, 1.0f),
        rgba(1.0f, 0.0f, 1.0f, 1.0f),
        rgba(1.0f, 1.0f, 0.0f, 1.0f),
    };

    std::vector<rbp::RectSize> rectSizes;

    float angle = 0.0f;

    for (i = 0; i < n; i++){
        float radius = inner_radius + angle * delta_radius;

        float cx = screen_width*0.5f + cosf(angle)*radius;
        float cy = screen_height*0.5f + sinf(angle)*radius;

        float r = box_outer_radius;
        float s = box_inner_radius;

        angle += box_grow_radius / radius;

        Shape shape;
        uint32_t color = nice_colors[i % nice_colors.size()];
        shape.color = color;
        make_rect(shape.vertices, cx - r, cy - r, cx + r, cy + r);
        make_rect(shape.vertices, cx - s, cy - s, cx + s, cy + s);
        for (Vertex &vertex : shape.vertices) vertex.color = color;

        shape.update();
        shapes.push_back(shape);

        rbp::RectSize rs;
        rs.width = shape.rect.width;
        rs.height = shape.rect.height;
        rs.index = i;
        rectSizes.push_back(rs);
    }

    std::vector<rbp::Rect> rects;
    rbp::MaxRectsBinPack pack(fbo_width, fbo_height);
    int err = pack.Insert(rectSizes, rects, rbp::MaxRectsBinPack::RectBottomLeftRule);
    if (err){
        printf("Failed to binpack so many rectangles\n");
        exit(err);
    }

    for (rbp::Rect rect : rects){
        shapes[rect.index].fbo_rect = rect;
    }

    for (const Shape &shape : shapes){
        rbp::Rect rect = shape.fbo_rect;
        for (Vertex vertex : shape.vertices){
            float x = vertex.x - shape.rect.x + rect.x;
            float y = vertex.y - shape.rect.y + rect.y;
            immediate_vertices.push_back(Vertex{x, y, 0.0f, 0.0f, vertex.color});
            direct_vertices.push_back(vertex);
        }
        int x0 = shape.rect.x;
        int y0 = shape.rect.y;
        int x1 = shape.rect.width + x0;
        int y1 = shape.rect.height + y0;
        float u0 = rect.x / (float)fbo_width;
        float v0 = rect.y / (float)fbo_height;
        float u1 = (rect.x + rect.width) / (float)fbo_width;
        float v1 = (rect.y + rect.height) / (float)fbo_height;
        make_rect(from_fbo_to_screen_vertices, x0, y0, x1, y1, u0, v0, u1, v1, shape.color);
        make_rect(cover_fbo_vertices, rect.x, rect.y, rect.x + rect.width, rect.y + rect.height, 0.0f, 0.0f, 1.0f, 1.0f, shape.color);
    }

    glGenBuffers(1, &vbo_immediate);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_immediate);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*immediate_vertices.size(), immediate_vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &vbo_cover_fbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cover_fbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*cover_fbo_vertices.size(), cover_fbo_vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &vbo_from_fbo_to_screen);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_from_fbo_to_screen);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*from_fbo_to_screen_vertices.size(), from_fbo_to_screen_vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &vbo_direct);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_direct);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*direct_vertices.size(), direct_vertices.data(), GL_STATIC_DRAW);
}

int main(int argc, char **argv) {
    glutInitWindowSize(screen_width, screen_height);
    glutInit(&argc, argv);
    glutCreateWindow("Benchmark");
    glutDisplayFunc(display);
    glutIdleFunc(display);

    GLenum status = glewInit();
    if (status != GLEW_OK) {
        printf("OpenGL Extension Wrangler (GLEW) failed to initialize");
        exit(-1);
    }

    glGenQueries(1, &timeElapsedQuery);
    color_texture = make_texture(fbo_width, fbo_height);
    stencil_texture = make_stencil_texture(fbo_width, fbo_height);
    make_fbo();

    const char *vert_src = STR(
        attribute vec4 a_data0;
        attribute vec4 a_data1;

        varying vec4 v_data0;
        varying vec4 v_data1;

        uniform mat4 u_0;

        void main(){
            v_data0 = a_data0;
            v_data1 = a_data1;

            gl_Position = u_0*vec4(a_data0.xy, 0.0, 1.0);
        }
    );
    const char *texture_frag_src = STR(
        varying vec4 v_data0;
        varying vec4 v_data1;

        uniform sampler2D u_1;

        void main(){
            gl_FragColor = texture2D(u_1, v_data0.zw) * v_data1;
        }
    );
    const char *color_frag_src = STR(
        varying vec4 v_data0;
        varying vec4 v_data1;

        void main(){
            gl_FragColor = v_data1;
        }
    );

    texture_shader = new Shader(vert_src, texture_frag_src);
    color_shader = new Shader(vert_src, color_frag_src);

    init_vertices();

    CHECK_GL
    glutMainLoop();
    return 0;
}

#include "Rect.cpp"
#include "MaxRectsBinPack.cpp"
