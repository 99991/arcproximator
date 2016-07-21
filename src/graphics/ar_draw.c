#include "ar_draw.h"

void ar_draw(const struct ar_vertex *vertices, int n_vertices, GLenum mode, GLint apos, GLint atex, GLint acol){
    GLuint vbo;

    if (n_vertices == 0) return;

    ar_glGenBuffers(1, &vbo);
    ar_glBindBuffer(GL_ARRAY_BUFFER, vbo);
    ar_glBufferData(GL_ARRAY_BUFFER, sizeof(*vertices)*n_vertices, vertices, GL_STATIC_DRAW);
    /* TODO passing the attributes here is weird */
    /* TODO don't create new vbo every time */

/*
    ar_glBufferSubData(AR_GL_ARRAY_BUFFER, 0, sizeof(*vertices)*n_vertices, vertices);
*/
    if (apos != -1){
        ar_glEnableVertexAttribArray(apos);
        ar_glVertexAttribPointer(apos, 2, GL_FLOAT, GL_FALSE, sizeof(*vertices), (char*)0);
    }

    if (atex != -1){
        ar_glEnableVertexAttribArray(atex);
        ar_glVertexAttribPointer(atex, 2, GL_FLOAT, GL_FALSE, sizeof(*vertices), (char*)8);
    }

    if (acol != -1){
        ar_glEnableVertexAttribArray(acol);
        ar_glVertexAttribPointer(acol, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(*vertices), (char*)16);
    }

    ar_glDrawArrays(mode, 0, n_vertices);

    ar_glDeleteBuffers(1, &vbo);
}
