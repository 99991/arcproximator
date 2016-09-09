#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
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

#define CHECK check_error(__LINE__);

GLuint fbo;
GLuint texture;
GLuint stencil_texture;
GLuint timeElapsedQuery;

int screen_width = 800;
int screen_height = 800;

int fbo_width = 1024;
int fbo_height = 1024;

void check_error(int line){
    GLenum error = glGetError();
    if (error != GL_NO_ERROR){
        printf("ERROR in line %d: %s\n", line, (const char*)gluErrorString(error));
    }
}

void make_texture(void){
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fbo_width, fbo_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
}

void make_stencil_texture(void){
    glGenTextures(1, &stencil_texture);

    glBindTexture(GL_TEXTURE_2D, stencil_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, fbo_width, fbo_height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
}

void make_fbo(void){
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, stencil_texture, 0);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

struct Vertex {
    float x, y, u, v;
    GLuint color;
};

struct BoundingBox {
    float x0, y0, x1, y1;

    void enclose(float x, float y){
        x0 = std::min(x0, x);
        y0 = std::min(y0, y);
        x1 = std::max(x1, x);
        y1 = std::max(y1, y);
    }
};

typedef std::vector<Vertex> Vertices;

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

#define PI_F 3.14159265358979f

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

void display(void){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearStencil(0);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    int i, n = 500;
    std::vector<Shape> shapes;

    int n_colors = 6;
    uint32_t nice_colors[6] = {
        0xFFFF0000, 0xFF00FF00, 0xFF0000FF,
        0xFF00FFFF, 0xFFFF00FF, 0xFFFFFF00
    };

    srand(0);
    std::vector<rbp::RectSize> rectSizes;

    float angle = 0.0f;
    float distance = 15.0f;

    for (i = 0; i < n; i++){
        float delta_radius = 10.0f;
        float radius = 50.0f + angle * delta_radius;

        float cx = 400.0f + cosf(angle)*radius;
        float cy = 400.0f + sinf(angle)*radius;
        float r = 20.0f;
        float s = r*0.5f;

        angle += distance / radius;

        Shape shape;
        uint32_t color = nice_colors[i % n_colors];
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
    pack.Insert(rectSizes, rects, rbp::MaxRectsBinPack::RectBottomLeftRule);

    Vertices fbo_vertices, fbo_to_screen_vertices;
    for (rbp::Rect rect : rects){
        shapes[rect.index].fbo_rect = rect;
    }

    for (const Shape &shape : shapes){
        rbp::Rect rect = shape.fbo_rect;
        for (Vertex vertex : shape.vertices){
            float x = vertex.x - shape.rect.x + rect.x;
            float y = vertex.y - shape.rect.y + rect.y;
            fbo_vertices.push_back(Vertex{x, y, 0.0f, 0.0f, vertex.color});
        }
        int x0 = shape.rect.x;
        int y0 = shape.rect.y;
        int x1 = shape.rect.width + x0;
        int y1 = shape.rect.height + y0;
        float u0 = rect.x / (float)fbo_width;
        float v0 = rect.y / (float)fbo_height;
        float u1 = (rect.x + rect.width) / (float)fbo_width;
        float v1 = (rect.y + rect.height) / (float)fbo_height;
        make_rect(fbo_to_screen_vertices, x0, y0, x1, y1, u0, v0, u1, v1, shape.color);
    }

    glBeginQuery(GL_TIME_ELAPSED, timeElapsedQuery);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, fbo_width, 0, fbo_height, -1, +1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fbo_width, fbo_height);
    glClearStencil(0);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_STENCIL_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glStencilFunc(GL_NEVER, 0, 1);
    glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), &fbo_vertices[0].color);
    glVertexPointer(2, GL_FLOAT, sizeof(Vertex), &fbo_vertices[0].x);
    glDrawArrays(GL_TRIANGLES, 0, fbo_vertices.size());

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilFunc(GL_EQUAL, 1, 1);
    glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);

    glDrawArrays(GL_TRIANGLES, 0, fbo_vertices.size());

    glDisable(GL_STENCIL_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, screen_width, screen_height);
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, screen_width, 0, screen_height, -1, +1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
#if 0
    fbo_to_screen_vertices.clear();
    make_rect(fbo_to_screen_vertices, 0, 0, screen_width, screen_height);
#endif
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), &fbo_to_screen_vertices[0].color);
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &fbo_to_screen_vertices[0].u);
    glVertexPointer(2, GL_FLOAT, sizeof(Vertex), &fbo_to_screen_vertices[0].x);
    glDrawArrays(GL_TRIANGLES, 0, fbo_to_screen_vertices.size());
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    CHECK
#if 0

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_STENCIL_TEST);
    for (i = 0; i < n; i++){

        /* prepare writing to stencil buffer */
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glStencilFunc(GL_NEVER, 0, 1);
        glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);

        glDrawArrays(GL_TRIANGLES, i*2*VERTS_PER_RECT, 2*VERTS_PER_RECT);

        /* prepare coloring stencil buffer */
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glStencilFunc(GL_EQUAL, 1, 1);
        glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);

        glDrawArrays(GL_TRIANGLES, i*2*VERTS_PER_RECT, VERTS_PER_RECT);

    }
    glDisable(GL_STENCIL_TEST);

#endif
    glEndQuery(GL_TIME_ELAPSED);
    glutSwapBuffers();

    uint64_t t;
    glGetQueryObjectui64v(timeElapsedQuery, GL_QUERY_RESULT, &t);
    printf("%i %f\n", n, t*1e-6);

    CHECK
}

void work(int frame) {
    glutPostRedisplay();
    glutTimerFunc(20, work, frame + 1);
}

int main(int argc, char **argv) {
    glutInitWindowSize(screen_width, screen_height);
    glutInit(&argc, argv);
    glutCreateWindow("Benchmark");
    glutDisplayFunc(display);

    GLenum status = glewInit();
    if (status != GLEW_OK) {
        printf("OpenGL Extension Wrangler (GLEW) failed to initialize");
        exit(-1);
    }

    glGenQueries(1, &timeElapsedQuery);
    make_texture();
    make_stencil_texture();
    make_fbo();

    CHECK
    work(0);
    glutMainLoop();
    return 0;
}

#include "Rect.cpp"
#include "MaxRectsBinPack.cpp"
