#ifndef MAT4_INCLUDED
#define MAT4_INCLUDED

typedef struct {
    double data[16];
} mat4;

#define m4at(a, i, j) a.data[i + j*4]

INLINE mat4 m4(
    double m00, double m01, double m02, double m03,
    double m10, double m11, double m12, double m13,
    double m20, double m21, double m22, double m23,
    double m30, double m31, double m32, double m33
){
    mat4 m;

    /* first row */
    m4at(m, 0, 0) = m00;
    m4at(m, 0, 1) = m01;
    m4at(m, 0, 2) = m02;
    m4at(m, 0, 3) = m03;

    /* second row */
    m4at(m, 1, 0) = m10;
    m4at(m, 1, 1) = m11;
    m4at(m, 1, 2) = m12;
    m4at(m, 1, 3) = m13;

    /* third row */
    m4at(m, 2, 0) = m20;
    m4at(m, 2, 1) = m21;
    m4at(m, 2, 2) = m22;
    m4at(m, 2, 3) = m23;

    /* fourth row */
    m4at(m, 3, 0) = m30;
    m4at(m, 3, 1) = m31;
    m4at(m, 3, 2) = m32;
    m4at(m, 3, 3) = m33;

    return m;
}

INLINE mat4 m4m23(mat23 a){
    return m4(
        m23at(a, 0, 0), m23at(a, 0, 1), 0.0, m23at(a, 0, 2),
        m23at(a, 1, 0), m23at(a, 1, 1), 0.0, m23at(a, 1, 2),
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

#endif
