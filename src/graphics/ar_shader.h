#ifndef AR_SHADER_INCLUDED
#define AR_SHADER_INCLUDED

#include "ar_gl.h"

struct ar_shader {
    GLuint program;
    GLuint vertex_shader;
    GLuint fragment_shader;
};

void ar_shader_init(
    struct ar_shader *shader,
    const char *vert_src,
    const char *frag_src
);

void ar_shader_use(struct ar_shader *shader);

#endif
