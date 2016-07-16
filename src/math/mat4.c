#include "mat4.h"

mat4 m4(
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

mat4 m4id(void){
    return m4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

mat4 m4m23(mat23 a){
    return m4(
        m23at(a, 0, 0), m23at(a, 0, 1), 0.0, m23at(a, 0, 2),
        m23at(a, 1, 0), m23at(a, 1, 1), 0.0, m23at(a, 1, 2),
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

mat4 m4_ortho2d(float l, float r, float b, float t){
    /* can not use "near" and "far" variable names because of windows.h */
    float n = +1.0f;
    float f = -1.0f;

    return m4(
        2.0f/(r - l), 0.0f, 0.0f, (r + l)/(l - r),
        0.0f, 2.0f/(t - b), 0.0f, (t + b)/(b - t),
        0.0f, 0.0f, 2.0f/(n - f), (f + n)/(n - f),
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

mat4 m4mul(mat4 a, mat4 b){
    mat4 c;
    int i, j;
    for (i = 0; i < 4; i++) for (j = 0; j < 4; j++){
        m4at(c, i, j) =
            m4at(a, i, 0)*m4at(b, 0, j) +
            m4at(a, i, 1)*m4at(b, 1, j) +
            m4at(a, i, 2)*m4at(b, 2, j) +
            m4at(a, i, 3)*m4at(b, 3, j);
    }
    return c;
}
