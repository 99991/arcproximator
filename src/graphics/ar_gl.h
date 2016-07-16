#ifndef AR_GL_INCLUDED
#define AR_GL_INCLUDED

#ifdef _WIN32

#include <GL/freeglut.h>
#include "../glad/glad.h"

#elif __APPLE__

#import <OpenGLES/ES2/glext.h>

#else
/* probably linux */

#include <GL/freeglut.h>
#include <glad/glad.h>

#endif

void ar_gl_check(int line);

#define AR_GL_CHECK ar_gl_check(__LINE__);

#endif
