#include "ar_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ar_is_good_double(double x){
    return
        (x >= x || x <= x) &&
        (x < +AR_DBL_INF) &&
        (x > -AR_DBL_INF);
}

void ar_print_bad_double(double x, int line, const char *path){
    printf("WARNING: %f in line %i, file %s\n", x, line, path);
    exit(-1);
}

double ar_clamp(double x, double a, double b){
    if (x < a) return a;
    if (x > b) return b;
    return x;
}

void* ar_load_file(const char *path, size_t *n_bytes){
    FILE *fp = fopen(path, "rb");
    if (!fp) return NULL;
    fseek(fp, 0, SEEK_END);
    size_t n = ftell(fp);
    rewind(fp);
    void *bytes = malloc(n);
    fread(bytes, 1, n, fp);
    fclose(fp);
    if (n_bytes) *n_bytes = n;
    return bytes;
}

int ar_ord(char c){
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'z') return c - 'a' + 10;
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
    return -1;
}
