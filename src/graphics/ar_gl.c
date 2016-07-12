#include "ar_gl.h"

#include <stdio.h>

static const char* ar_gl_error_string(GLenum err){
    switch (err){
        case GL_NO_ERROR: return "GL_NO_ERROR";
        case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
        case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
        case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
        case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
        default: return "Unknown OpenGL error";
    }
}

void ar_gl_check(int line){
    GLenum err = glGetError();
    if (err != GL_NO_ERROR){
        printf("Line %i: %s\n", line, ar_gl_error_string(err));
    }
}
