#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>

#define PIF 3.14159265358979f

struct Vec2f {
    float x, y;
};

Vec2f operator + (Vec2f a, Vec2f b){
    return Vec2f{a.x + b.x, a.y + b.y};
}

Vec2f operator - (Vec2f a, Vec2f b){
    return Vec2f{a.x - b.x, a.y - b.y};
}

Vec2f operator * (float a, Vec2f b){
    return Vec2f{a*b.x, a*b.y};
}

Vec2f polar(float angle){
    return Vec2f{cosf(angle), sinf(angle)};
}

int w = 600;
int h = 600;

void draw(const void *data, int n, GLenum mode){
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, data);
    glDrawArrays(mode, 0, n);
}

void draw_circle(float x, float y, float r){
    size_t i, n = 100;
    float xy[2*100];
    for (i = 0; i < n; i++){
        float angle = 2.0f*3.14159f*i/n;
        xy[i*2 + 0] = x + cos(angle)*r;
        xy[i*2 + 1] = y + sin(angle)*r;
    }
    draw(xy, n, GL_LINE_LOOP);
}

std::vector<Vec2f> make_circle(Vec2f center, float radius, int n = 10){
    std::vector<Vec2f> points(n);

    for (int i = 0; i < n; i++){
        float angle = 2.0f*PIF*i/n + PIF*0.5f;
        points[i] = center + radius*polar(angle);
    }

    return points;
}

void on_frame(){
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1, +1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    Vec2f center{w*0.5f, h*0.5f};

    auto outer_circle = make_circle(center, 250.0f, 5);
    auto inner_circle = make_circle(center, 150.0f, 3);

#if 1
    glColor4f(0.0f, 0.0f, 0.0f, 0.2f);
    draw(outer_circle.data(), outer_circle.size(), GL_TRIANGLE_FAN);
    glColor3f(0.0f, 0.0f, 0.0f);
    draw(outer_circle.data(), outer_circle.size(), GL_LINE_LOOP);

    glColor3f(1.0f, 1.0f, 1.0f);
    draw(inner_circle.data(), inner_circle.size(), GL_TRIANGLE_FAN);
    glColor3f(0.0f, 0.0f, 0.0f);
    draw(inner_circle.data(), inner_circle.size(), GL_LINE_LOOP);
#else
    Vec2f pivot{50.0f, 200.0f};

    Vec2f a = outer_circle.back();
    for (Vec2f b : outer_circle){
        Vec2f tri[3] = {
            pivot, a, b,
        };
        glColor4f(0.0f, 0.0f, 0.0f, 0.2f);
        draw(tri, 3, GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        draw(tri, 3, GL_LINE_LOOP);
        a = b;
    }

    a = inner_circle.back();
    for (Vec2f b : inner_circle){
        Vec2f tri[3] = {
            pivot, a, b,
        };
        glColor4f(0.0f, 0.0f, 0.0f, 0.2f);
        draw(tri, 3, GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        draw(tri, 3, GL_LINE_LOOP);
        a = b;
    }
#endif
    glutSwapBuffers();
}

void work(int frame){
    glutPostRedisplay();
    glutTimerFunc(20, work, frame + 1);
}

int main(int argc, char **argv){

    glutInit(&argc, argv);
    glutSetOption(GLUT_MULTISAMPLE, 8);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(w, h);
    glutCreateWindow("");

    glewInit();

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glutDisplayFunc(on_frame);
    work(0);
    glutMainLoop();
    return 0;
}
