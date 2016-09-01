#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

GLuint path;

char* load_text(const char *path, size_t *n_bytes){
    FILE *fp = fopen(path, "rb");
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

void init(){
    glClearStencil(0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 0, 0x1F);
    glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);

    path = glGenPathsNV(1);
    size_t n;
    char *path_src = load_text("path.txt", &n);
    glPathStringNV(path, GL_PATH_FORMAT_SVG_NV, (GLsizei)n, path_src);
    free(path_src);
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 800, 800, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    GLuint timeElapsedQuery;
    glGenQueries(1, &timeElapsedQuery);
    glBeginQuery(GL_TIME_ELAPSED, timeElapsedQuery);

    glStencilFillPathNV(path, GL_INVERT, 0x1F);
    glCoverFillPathNV(path, GL_BOUNDING_BOX_NV);

    glEndQuery(GL_TIME_ELAPSED);
    glutSwapBuffers();

    uint64_t t;
    glGetQueryObjectui64v(timeElapsedQuery, GL_QUERY_RESULT, &t);
    printf("Time Elapsed: %f milliseconds\n", t*1e-6);
    glDeleteQueries(1, &timeElapsedQuery);
}

void work(int frame) {
    glutPostRedisplay();
    glutTimerFunc(20, work, frame + 1);
}

int main(int argc, char **argv) {
    glutInitWindowSize(800, 800);
    glutInit(&argc, argv);
    glutCreateWindow("Benchmark");
    printf("vendor: %s\n", glGetString(GL_VENDOR));
    printf("version: %s\n", glGetString(GL_VERSION));
    printf("renderer: %s\n", glGetString(GL_RENDERER));
    printf("samples per pixel = %d\n", glutGet(GLUT_WINDOW_NUM_SAMPLES));

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

    init();
    work(0);
    glutMainLoop();
    return 0;
}

