#ifndef AR_TEXTURE_INCLUDED
#define AR_TEXTURE_INCLUDED

#include "ar_gl.h"

struct ar_texture {
    GLuint id;
    int width;
    int height;
};

void ar_texture_init(struct ar_texture *texture, int width, int height, const void *data);
void ar_texture_bind(struct ar_texture *texture);
void ar_texture_free(struct ar_texture *texture);
void ar_texture_linear(struct ar_texture *texture);
void ar_texture_init_stencil(struct ar_texture *texture, int width, int height);

#endif
