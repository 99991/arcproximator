#ifndef AR_GL_INCLUDED
#define AR_GL_INCLUDED

/* TODO replace glew */

#ifdef _WIN32

#include <GL/glew.h>
#include <GL/freeglut.h>

#elif __APPLE__

#import <OpenGLES/ES2/glext.h>

#else
/* probably linux */

#include "../GL/glew.h"
#include <GL/freeglut.h>

#endif

void ar_gl_check(int line);

#define AR_GL_CHECK ar_gl_check(__LINE__);

#endif
