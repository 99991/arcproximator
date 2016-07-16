#include "mat23.h"

mat23 m23(
    double m00, double m01, double m02,
    double m10, double m11, double m12
){
    mat23 c;

    /* first row */
    m23at(c, 0, 0) = m00;
    m23at(c, 0, 1) = m01;
    m23at(c, 0, 2) = m02;

    /* second row */
    m23at(c, 1, 0) = m10;
    m23at(c, 1, 1) = m11;
    m23at(c, 1, 2) = m12;

    return c;
}

mat23 m23id(void){
    return m23(
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0
    );
}

vec2 m23mulv2(mat23 a, vec2 b){
    double x = m23at(a, 0, 0)*b.x + m23at(a, 0, 1)*b.y + m23at(a, 0, 2);
    double y = m23at(a, 1, 0)*b.x + m23at(a, 1, 1)*b.y + m23at(a, 1, 2);
    return v2(x, y);
}

mat23 m23mul(mat23 lhs, mat23 rhs){
    /* matrices have the form:  */
    /*                          */
    /*   [a b c]   [q r s]      */
    /*   [d e f] * [t u v]      */
    /*   [0 0 1]   [0 0 1]      */

    double a = m23at(lhs, 0, 0);
    double b = m23at(lhs, 0, 1);
    double c = m23at(lhs, 0, 2);
    double d = m23at(lhs, 1, 0);
    double e = m23at(lhs, 1, 1);
    double f = m23at(lhs, 1, 2);

    double q = m23at(rhs, 0, 0);
    double r = m23at(rhs, 0, 1);
    double s = m23at(rhs, 0, 2);
    double t = m23at(rhs, 1, 0);
    double u = m23at(rhs, 1, 1);
    double v = m23at(rhs, 1, 2);

    return m23(
        a*q + b*t, a*r + b*u, a*s + b*v + c,
        d*q + e*t, d*r + e*u, d*s + e*v + f
    );
}

mat23 m23inv(mat23 u){
    double a = m23at(u, 0, 0);
    double b = m23at(u, 0, 1);
    double x = m23at(u, 0, 2);
    double c = m23at(u, 1, 0);
    double d = m23at(u, 1, 1);
    double y = m23at(u, 1, 2);

    double inv_det = 1.0/(a*d - b*c);

    return m23(
        +inv_det*d, -inv_det*b, inv_det*(b*y - d*x),
        -inv_det*c, +inv_det*d, inv_det*(c*x - a*y)
    );
}
