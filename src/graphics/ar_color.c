#include "ar_color.h"
#include "../util/ar_util.h"

#include <stdio.h>
#include <string.h>

uint32_t ar_color_from_hex_n(const char *hex, size_t n){
    if (n > 0 && *hex == '#') hex++;
    n--;
    if (n == 3){
        uint32_t r = ar_ord(*hex++);
        uint32_t g = ar_ord(*hex++);
        uint32_t b = ar_ord(*hex++);
        r |= r << 4;
        g |= g << 4;
        b |= b << 4;
        return 0xff000000 | (b << 16) | (g << 8) | r;
    }else if (n == 6){
        uint32_t r = ar_ord(*hex++);
        r = (r << 4) | ar_ord(*hex++);
        uint32_t g = ar_ord(*hex++);
        g = (g << 4) | ar_ord(*hex++);
        uint32_t b = ar_ord(*hex++);
        b = (b << 4) | ar_ord(*hex++);
        return 0xff000000 | (b << 16) | (g << 8) | r;
    }else{
        fprintf(stderr, "Unexpected hex color string: %.*s\n", n, hex);
        return 0;
    }
}

uint32_t ar_color_from_hex(const char *hex){
    return ar_color_from_hex_n(hex, strlen(hex));
}

void ar_color_to_float(uint32_t color, float *rgba){
    rgba[0] = ((color >> 0*8) & 0xff) / 255.0f;
    rgba[1] = ((color >> 1*8) & 0xff) / 255.0f;
    rgba[2] = ((color >> 2*8) & 0xff) / 255.0f;
    rgba[3] = ((color >> 3*8) & 0xff) / 255.0f;
}
