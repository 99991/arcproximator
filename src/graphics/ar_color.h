#ifndef AR_COLOR_INCLUDED
#define AR_COLOR_INCLUDED

#include <stdint.h>

#define AR_RGBA(r, g, b, a) (((uint32_t)a << 24) | ((uint32_t)b << 16) | ((uint32_t)g << 8) | (uint32_t)r)
#define AR_RGB(r, g, b) AR_RGBA(r, g, b, 0xff)

#define AR_PINK       0xffff00ff
#define AR_YELLOW     0xff00ffff
#define AR_LIGHT_BLUE 0xffffff00
#define AR_RED        0xff0000ff
#define AR_GREEN      0xff00ff00
#define AR_BLUE       0xffff0000
#define AR_WHITE      0xffffffff
#define AR_BLACK      0xff000000

#endif
