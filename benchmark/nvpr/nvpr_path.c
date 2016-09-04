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

GLuint path;

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

char* load_text(const char *path, size_t *n_bytes){
    FILE *fp = fopen(path, "rb");
    assert(fp);
    fseek(fp, 0, SEEK_END);
    size_t n = ftell(fp);
    rewind(fp);
    char *data = malloc(n + 1);
    fread(data, 1, n, fp);
    data[n] = '\0';
    if (n_bytes) *n_bytes = n;
    fclose(fp);
    return data;
}

void init_nvpr(void){
    CHECK
    glClearStencil(0);
    CHECK
    glStencilFunc(GL_NOTEQUAL, 0, 0x1F);
    glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
    CHECK
    path = glGenPathsNV(1);
    size_t n;
    char *path_src = load_text("../../development/py/path.txt", &n);
    glPathStringNV(path, GL_PATH_FORMAT_SVG_NV, (GLsizei)n, path_src);
    free(path_src);
    CHECK
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

void cleanup(void){
    glDeleteTextures(1, &texture);
    glDeleteTextures(1, &stencil_texture);
    glDeleteFramebuffers(1, &fbo);
}

void display(void){
    static int pixels_increment = 256*256;
    static int pixels = 0;
    pixels += pixels_increment;

    if (pixels > 4096*4096){
        exit(0);
    }

    width = sqrt(pixels);
    height = sqrt(pixels);

    make_texture();
    make_stencil_texture();
    make_fbo();
    render_to_fbo();
    render_from_fbo_to_screen();
    glutSwapBuffers();

    uint64_t t;
    glGetQueryObjectui64v(timeElapsedQuery, GL_QUERY_RESULT, &t);
    printf("%i %i %f\n", width, height, t*1e-6);
    glDeleteQueries(1, &timeElapsedQuery);

    cleanup();
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
#if 0
    printf("vendor: %s\n", glGetString(GL_VENDOR));
    printf("version: %s\n", glGetString(GL_VERSION));
    printf("renderer: %s\n", glGetString(GL_RENDERER));
#endif
    glutDisplayFunc(display);

    GLenum status = glewInit();
    if (status != GLEW_OK) {
        printf("OpenGL Extension Wrangler (GLEW) failed to initialize");
        exit(-1);
    }
    GLboolean hasDSA = glewIsSupported("GL_EXT_direct_state_access");
    if (!hasDSA) {
        printf("OpenGL implementation doesn't support GL_EXT_direct_state_access (you should be using NVIDIA GPUs...)");
        exit(-1);
    }
    CHECK
    init_nvpr();
    work(0);
    glutMainLoop();
    return 0;
}

