#ifndef COLOR_INCLUDED
#define COLOR_INCLUDED

#define SET_COLOR(color) glColor4ub((color >> 0*8) & 0xff, (color >> 1*8) & 0xff, (color >> 2*8) & 0xff, (color >> 3*8) & 0xff);

#define PINK       0xffff00ff
#define YELLOW     0xff00ffff
#define LIGHT_BLUE 0xffffff00
#define RED        0xff0000ff
#define GREEN      0xff00ff00
#define BLUE       0xffff0000
#define WHITE      0xffffffff
#define BLACK      0xff000000
#define DARK_GRAY  0xff555555

#endif
