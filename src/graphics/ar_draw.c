#include "ar_draw.h"

void ar_draw(const struct ar_vertex *vertices, int n_vertices, GLenum mode, GLint apos, GLint atex, GLint acol){
    if (n_vertices == 0) return;

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(*vertices)*n_vertices, vertices, GL_STATIC_DRAW);
    /* TODO passing the attributes here is weird */
    /* TODO don't create new vbo every time */

/*
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(*vertices)*n_vertices, vertices);
*/
    if (apos != -1){
        glEnableVertexAttribArray(apos);
        glVertexAttribPointer(apos, 2, GL_FLOAT, GL_FALSE, sizeof(*vertices), (char*)0);
    }

    if (atex != -1){
        glEnableVertexAttribArray(atex);
        glVertexAttribPointer(atex, 2, GL_FLOAT, GL_FALSE, sizeof(*vertices), (char*)8);
    }

    if (acol != -1){
        glEnableVertexAttribArray(acol);
        glVertexAttribPointer(acol, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(*vertices), (char*)16);
    }

    glDrawArrays(mode, 0, n_vertices);

    glDeleteBuffers(1, &vbo);
}
