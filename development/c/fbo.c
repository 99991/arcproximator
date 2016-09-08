#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define CHECK check_error(__LINE__);

GLuint fbo;
GLuint texture;
GLuint stencil_texture;
GLuint timeElapsedQuery;

int width = 64;
int height = 64;

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
}

void make_stencil_texture(void){
    glGenTextures(1, &stencil_texture);

    glBindTexture(GL_TEXTURE_2D, stencil_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
}

void make_fbo(void){
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, stencil_texture, 0);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}
/*
void render_to_fbo(void){
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glColor3f(0.0f, 0.0f, 0.0f);
    glEnable(GL_STENCIL_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 800, 800, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glGenQueries(1, &timeElapsedQuery);
    glBeginQuery(GL_TIME_ELAPSED, timeElapsedQuery);

    glStencilFillPathNV(path, GL_INVERT, 0x1F);
    glCoverFillPathNV(path, GL_BOUNDING_BOX_NV);

    glEndQuery(GL_TIME_ELAPSED);

    glDisable(GL_STENCIL_TEST);
}

void render_from_fbo_to_screen(void){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 800, 800);

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glColor3f(1.0f, 1.0f, 1.0f);

#if 0
    glGenerateMipmap(GL_TEXTURE_2D);
#endif

    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0, 0); glVertex2f(-1, -1);
    glTexCoord2f(1, 0); glVertex2f(+1, -1);
    glTexCoord2f(1, 1); glVertex2f(+1, +1);
    glTexCoord2f(0, 1); glVertex2f(-1, +1);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}
*/
void cleanup(void){
    glDeleteTextures(1, &texture);
    glDeleteTextures(1, &stencil_texture);
    glDeleteFramebuffers(1, &fbo);
}

void make_rect(float *pos, float x0, float y0, float x1, float y1){
    *pos++ = x0; *pos++ = y0;
    *pos++ = x1; *pos++ = y0;
    *pos++ = x1; *pos++ = y1;

    *pos++ = x0; *pos++ = y0;
    *pos++ = x1; *pos++ = y1;
    *pos++ = x0; *pos++ = y1;
}

void foo(void){
    glEnable(GL_STENCIL_TEST);
    glClearStencil(0);

    /* prepare writing to stencil buffer */
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glStencilFunc(GL_NEVER, 0, 1);
    glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);

    float pos[4*3];
    make_rect(pos + 0*3, 0.0f, 0.0f, 0.8f, 0.8f);
    make_rect(pos + 2*3, 0.1f, 0.1f, 0.7f, 0.7f);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, pos);
    glDrawArrays(GL_TRIANGLES, 0, 4*3);

    /* prepare coloring stencil buffer */
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilFunc(GL_EQUAL, 1, 1);
    glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);

    glDrawArrays(GL_TRIANGLES, 0, 2*3);

    glDisable(GL_STENCIL_TEST);
}

void display(void){
    make_texture();
    make_stencil_texture();
    make_fbo();
    //render_to_fbo();
    //render_from_fbo_to_screen();

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    foo();

    glutSwapBuffers();

    uint64_t t;
    glGetQueryObjectui64v(timeElapsedQuery, GL_QUERY_RESULT, &t);
    printf("%i %i %f\n", width, height, t*1e-6);
    glDeleteQueries(1, &timeElapsedQuery);

    //cleanup();
    CHECK
}

void work(int frame) {
    glutPostRedisplay();
    glutTimerFunc(20, work, frame + 1);
}

int main(int argc, char **argv) {
    glutInitWindowSize(800, 800);
    glutInit(&argc, argv);
    glutCreateWindow("Benchmark");
    glutDisplayFunc(display);

    GLenum status = glewInit();
    if (status != GLEW_OK) {
        printf("OpenGL Extension Wrangler (GLEW) failed to initialize");
        exit(-1);
    }

    CHECK
    work(0);
    glutMainLoop();
    return 0;
}

