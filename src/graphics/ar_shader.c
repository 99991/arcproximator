#include "ar_shader.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int shader_compile_check(GLuint shader){
    char *log;
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        log = (char*)malloc(length);
        glGetShaderInfoLog(shader, length, &length, log);
        puts(log);
        free(log);
        return 0;
    }
    return 1;
}

static int shader_link_check(GLuint program){
    char *log;
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        log = (char*)malloc(length);
        glGetProgramInfoLog(program, length, &length, log);
        puts(log);
        free(log);
        return 0;
    }
    return 1;
}

static GLuint shader_compile(const char *source, GLenum type){
    GLuint shader = glCreateShader(type);
    int length = (int)strlen(source);
    glShaderSource(shader, 1, &source, &length);
    glCompileShader(shader);
    if(!shader_compile_check(shader)) exit(0);
    return shader;
}

void ar_shader_init(
    struct ar_shader *shader,
    const char *vert_src,
    const char *frag_src
){
    shader->vertex_shader = shader_compile(vert_src, GL_VERTEX_SHADER);
    shader->fragment_shader = shader_compile(frag_src, GL_FRAGMENT_SHADER);
    shader->program = glCreateProgram();
    glAttachShader(shader->program, shader->vertex_shader);
    glAttachShader(shader->program, shader->fragment_shader);
    glLinkProgram(shader->program);
    if(!shader_link_check(shader->program)) exit(-1);
}

void ar_shader_use(struct ar_shader *shader){
    glUseProgram(shader->program);
}
