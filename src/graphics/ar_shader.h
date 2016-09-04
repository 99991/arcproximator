#ifndef AR_SHADER_INCLUDED
#define AR_SHADER_INCLUDED

#include "ar_gl.h"

#include "../math/mat4.h"

#define AR_MAX_ATTRIBUTES 4
#define AR_MAX_UNIFORMS   2

struct ar_shader {
    GLuint program;
    GLuint vertex_shader;
    GLuint fragment_shader;

    GLint uniforms[AR_MAX_ATTRIBUTES];
    GLint attributes[AR_MAX_ATTRIBUTES];
};

void ar_shader_init(
    struct ar_shader *shader,
    const char *vert_src,
    const char *frag_src
);

void ar_shader_use(struct ar_shader *shader);

void ar_upload_model_view_projection(struct ar_shader *shader, mat4 mvp);
#endif
