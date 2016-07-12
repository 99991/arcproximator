#ifndef MAT2_INCLUDED
#define MAT2_INCLUDED

#include "vec2.h"

typedef struct {
    double data[4];
} mat2;

#define m2at(a, i, j) a.data[i + j*2]

INLINE vec2 m2row0(mat2 a){
    return v2(m2at(a, 0, 0), m2at(a, 0, 1));
}

INLINE vec2 m2row1(mat2 a){
    return v2(m2at(a, 1, 0), m2at(a, 1, 1));
}

INLINE vec2 m2col0(mat2 a){
    return v2(m2at(a, 0, 0), m2at(a, 1, 0));
}

INLINE vec2 m2col1(mat2 a){
    return v2(m2at(a, 0, 1), m2at(a, 1, 1));
}

INLINE mat2 m2(double m00, double m01, double m10, double m11){
    mat2 m;

    m2at(m, 0, 0) = m00;
    m2at(m, 0, 1) = m01;
    m2at(m, 1, 0) = m10;
    m2at(m, 1, 1) = m11;

    return m;
}

INLINE mat2 m2scale(double s){
    return m2(
        s, 0.0,
        0.0, s
    );
}

INLINE mat2 m2rot(double angle){
    double c = cos(angle);
    double s = sin(angle);

    return m2(
        c, -s,
        s, c
    );
}

INLINE mat2 m2id(){
    return m2scale(1.0);
}

INLINE vec2 m2mulv2(mat2 a, vec2 b){
    return v2(v2dot(m2row0(a), b), v2dot(m2row1(a), b));
}

INLINE mat2 m2mulm2(mat2 a, mat2 b){
    return m2(
        v2dot(m2row0(a), m2col0(b)), v2dot(m2row0(a), m2col1(b)),
        v2dot(m2row1(a), m2col0(b)), v2dot(m2row1(a), m2col1(b))
    );
}

INLINE double m2det(mat2 a){
    return v2det(m2row0(a), m2row1(a));
}

INLINE mat2 m2inv(mat2 m){
    double a = m2at(m, 0, 0);
    double b = m2at(m, 0, 1);
    double c = m2at(m, 1, 0);
    double d = m2at(m, 1, 1);

    double inv_det = 1.0/(a*d - b*c);

    return m2(
        +inv_det*d, -inv_det*b,
        -inv_det*c, +inv_det*a
    );
}

#endif // MAT2_INCLUDED
