#include "../graphics/ar_shader.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static GLuint nvpr_path;

static char* load_text(const char *path, size_t *n_bytes){
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

void prepare_nvpr(const char *path){
    nvpr_path = glGenPathsNV(1);
    size_t n;
    char *path_src = load_text(path, &n);
    glPathStringNV(nvpr_path, GL_PATH_FORMAT_SVG_NV, (GLsizei)n, path_src);
    free(path_src);
}

void upload_nvpr(void){
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 0, 0x1F);
    glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);

    glClearStencil(0);
    glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
}

void render_nvpr(void){
    glUseProgram(0);

    glStencilFillPathNV(nvpr_path, GL_INVERT, 0x1F);
    glCoverFillPathNV(nvpr_path, GL_BOUNDING_BOX_NV);

    glDisable(GL_STENCIL_TEST);
}
