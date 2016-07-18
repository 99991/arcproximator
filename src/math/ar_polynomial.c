#include "ar_polynomial.h"

double ar_polynomial_eval(double *coefficients, int n_coefficients, double x){
    double y = 0.0;

    int i;
    for (i = 0; i < n_coefficients; i++){
        y = y*x + coefficients[i];
    }

    return y;
}

/* TODO just for debugging */
#include <stdio.h>

/* The largest double value of 1.7976931348623157e+308 can be halved 2098 */
/* times until it is zero. */
#define AR_POLYNOMIAL_MAX_ITERATIONS 2100

double ar_polynomial_root(double *coefficients, int n_coefficients, double x0, double x1, double tolerance){
    int i;
    double dx = 0.5*(x1 - x0);
    double m = x0 + dx;
    double px0, px1, pm;

    x0 = m;
    x1 = m;

    for (i = 0; i < AR_POLYNOMIAL_MAX_ITERATIONS; i++){
        dx *= 0.5;
        x0 -= dx;
        x1 += dx;

        px0 = ar_polynomial_eval(coefficients, n_coefficients, x0);
        px1 = ar_polynomial_eval(coefficients, n_coefficients, x1);

        if (px0*px1 < 0.0) break;
    }

    if (i == AR_POLYNOMIAL_MAX_ITERATIONS){
        printf("WARNING: did not find bisection starting point in interval\n");
    }

    for (i = 0; i < AR_POLYNOMIAL_MAX_ITERATIONS; i++){
        dx = x1 - x0;
        m = x0 + dx*0.5;

        pm = ar_polynomial_eval(coefficients, n_coefficients, m);

        if (-tolerance < pm && pm < tolerance) return m;

        px0 = ar_polynomial_eval(coefficients, n_coefficients, x0);

        if (px0*pm > 0){
            x0 = m;
        }else{
            x1 = m;
        }
    }

    if (i == AR_POLYNOMIAL_MAX_ITERATIONS){
        printf("WARNING: did not converge below tolerance: %e\n", pm);
    }

    return m;
}
