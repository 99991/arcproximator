#include "../graphics/ar_shader.h"

void prepare_stencil_segments(const char *path);
void upload_stencil_segments(struct ar_shader *shader, GLuint vbo);
void render_stencil_segments();
