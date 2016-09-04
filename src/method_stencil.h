#include "graphics/ar_shader.h"

void prepare_svg(const char *path);
void upload_svg(struct ar_shader *shader, GLuint vbo);
void render_svg(struct ar_shader *shader, GLuint vbo, mat4 mvp, mat4 projection);
