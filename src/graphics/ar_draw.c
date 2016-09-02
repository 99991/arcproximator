#include "ar_draw.h"

void ar_enable_attributes(struct ar_shader *shader){
    ar_shader_use(shader);

    int i;
    for (i = 0; i < AR_MAX_ATTRIBUTES; i++){
        int location = shader->attributes[i];
        if (location != -1){
            glEnableVertexAttribArray(location);
        }
    }
}

void ar_disable_attributes(struct ar_shader *shader){
    ar_shader_use(shader);

    int i;
    for (i = 0; i < AR_MAX_ATTRIBUTES; i++){
        int location = shader->attributes[i];
        if (location != -1){
            glEnableVertexAttribArray(location);
        }
    }
}

void ar_set_attributes(struct ar_shader *shader, const struct ar_vertex *vertices){
    ar_shader_use(shader);

    int i;
    for (i = 0; i < AR_MAX_ATTRIBUTES; i++){
        int location = shader->attributes[i];
        if (location != -1){
            glEnableVertexAttribArray(location);
            if (!vertices) continue;
            glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, sizeof(*vertices), (char*)(i*16));
        }
    }
}

void ar_draw(struct ar_shader *shader, const struct ar_vertex *vertices, int n_vertices, GLenum mode, GLint vbo){
    if (n_vertices == 0) return;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(*vertices)*n_vertices, vertices);

    ar_set_attributes(shader, vertices);

    glDrawArrays(mode, 0, n_vertices);
}
