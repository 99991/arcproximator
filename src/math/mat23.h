#ifndef MAT23_INCLUDED
#define MAT23_INCLUDED

#include "vec2.h"

typedef struct {
    double data[6];
} mat23;

#define m23at(a, i, j) a.data[i + j*2]

mat23 m23(
    double m00, double m01, double m02,
    double m10, double m11, double m12
);
mat23 m23id();
vec2 m23mulv2(mat23 a, vec2 b);
mat23 m23mul(mat23 lhs, mat23 rhs);
mat23 m23inv(mat23 u);

#endif
