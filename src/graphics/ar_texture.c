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

/*
    TODO channels

    switch (n_channels){
    case 1: glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, image.data()); break;
    case 3: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB      , width, height, 0, GL_RGB      , GL_UNSIGNED_BYTE, image.data()); break;
    case 4: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA     , width, height, 0, GL_RGBA     , GL_UNSIGNED_BYTE, image.data()); break;
    default:
        printf("Failed to create texture from image with %i color channels\n", n_channels);
        break;
    }
    */
}

void ar_texture_bind(struct ar_texture *texture){
    glBindTexture(GL_TEXTURE_2D, texture->id);
}

void ar_texture_free(struct ar_texture *texture){
    glDeleteTextures(1, &texture->id);
}
