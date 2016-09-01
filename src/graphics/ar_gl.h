#ifndef gl_INCLUDED
#define gl_INCLUDED

#include <GL/glew.h>
#include <GL/freeglut.h>

void gl_check(int line);

#define GL_CHECK gl_check(__LINE__);

#endif
