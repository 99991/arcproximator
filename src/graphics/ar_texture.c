#include "ar_texture.h"

void ar_texture_init(struct ar_texture *texture, int width, int height, const void *data){
    texture->width = width;
    texture->height = height;
    glGenTextures(1, &texture->id);

    ar_texture_bind(texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

void ar_texture_linear(struct ar_texture *texture){
    ar_texture_bind(texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

void ar_texture_bind(struct ar_texture *texture){
    glBindTexture(GL_TEXTURE_2D, texture->id);
}

void ar_texture_free(struct ar_texture *texture){
    glDeleteTextures(1, &texture->id);
}

void ar_texture_init_stencil(struct ar_texture *texture, int width, int height){
    texture->width = width;
    texture->height = height;
    glGenTextures(1, &texture->id);

    ar_texture_bind(texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
}
