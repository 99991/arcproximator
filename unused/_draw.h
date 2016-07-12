#ifndef DRAW_INCLUDED
#define DRAW_INCLUDED

#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>

#include "mat23.h"
#include "mat4.h"

void draw_colored(const vec2 *vertices, const GLuint *colors, int n, GLenum mode){
    if (n == 0) return;

    float *floats = (float*)malloc(n*sizeof(*floats)*2);
    int i;
    for (i = 0; i < n; i++){
        floats[2*i + 0] = vertices[i].x;
        floats[2*i + 1] = vertices[i].y;
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, floats);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);

    glDrawArrays(mode, 0, n);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    free(floats);
}

void draw(const vec2 *vertices, int n, GLenum mode){
    if (n == 0) return;

    float *floats = (float*)malloc(n*sizeof(*floats)*2);
    int i;
    for (i = 0; i < n; i++){
        floats[2*i + 0] = vertices[i].x;
        floats[2*i + 1] = vertices[i].y;
    }

    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, floats);

    glDrawArrays(mode, 0, n);

    glDisableClientState(GL_VERTEX_ARRAY);

    free(floats);
}

void make_circle(vec2 *vertices, int n, vec2 center, double radius){
    double delta_angle = 2.0*PI/n;
    int i;
    for (i = 0; i < n; i++){
        double angle = i*delta_angle;
        vertices[i] = v2add(center, v2polar(angle, radius));
    }
}

void draw_circle(vec2 center, double radius){
    vec2 vertices[1000];
    make_circle(vertices, 1000, center, radius);
    draw(vertices, 1000, GL_LINE_LOOP);
}

void draw_line(vec2 a, vec2 b){
    vec2 vertices[2] = {a, b};
    draw(vertices, 2, GL_LINES);
}

void draw_arrow(vec2 a, vec2 b, double r){
    draw_line(a, b);
    vec2 d = v2scale(v2sub(b, a), r);
    vec2 triangle[3] = {
        b,
        v2sub(b, v2add(v2smul(2.0, d), v2left(d))),
        v2sub(b, v2sub(v2smul(2.0, d), v2left(d))),
    };
    draw(triangle, 3, GL_TRIANGLES);
}

void write(const char *text, double x, double y){
    glRasterPos2d(x, y);
    for (; *text; text++){
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *text);
    }
}

#endif
