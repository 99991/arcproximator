#ifndef AR_MESH_INCLUDED
#define AR_MESH_INCLUDED

#include "ar_gl.h"
#include "ar_vertex.h"
#include "ar_shader.h"

void ar_set_attributes(struct ar_shader *shader, const struct ar_vertex *vertices);
void ar_draw(struct ar_shader *shader, const struct ar_vertex *vertices, int n_vertices, GLenum mode, GLint vbo);
void ar_enable_attributes(struct ar_shader *shader);
void ar_disable_attributes(struct ar_shader *shader);

#endif
