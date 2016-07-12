#ifndef AR_MESH_INCLUDED
#define AR_MESH_INCLUDED

#include "ar_gl.h"
#include "ar_vertex.h"

void ar_draw(const struct ar_vertex *vertices, int n_vertices, GLenum mode, GLint apos, GLint atex, GLint acol);

#endif
