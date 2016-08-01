#include <stdio.h>
#include <stdint.h>

int ar_ord(char c){
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'z') return c - 'a' + 10;
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
    return -1;
}

uint32_t ar_color_from_hex(const char *hex, int n){
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

int main(){

    printf("%08x\n", ar_color_from_hex("#abcdef", 7));

    return 0;
}
