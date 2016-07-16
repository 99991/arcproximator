#ifndef MAT4_INCLUDED
#define MAT4_INCLUDED

#include "mat23.h"

typedef struct {
    double data[16];
} mat4;

/* column major matrix */
#define m4at(a, i, j) a.data[i + j*4]

mat4 m4(
    double m00, double m01, double m02, double m03,
    double m10, double m11, double m12, double m13,
    double m20, double m21, double m22, double m23,
    double m30, double m31, double m32, double m33
);

mat4 m4id();
mat4 m4m23(mat23 a);
mat4 m4_ortho2d(float l, float r, float b, float t);
mat4 m4mul(mat4 a, mat4 b);

#endif
