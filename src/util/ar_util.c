#include "ar_util.h"

#include <stdio.h>
#include <stdlib.h>

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
