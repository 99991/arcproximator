#include "ar_polynomial.h"

double ar_polynomial_eval(double *coefficients, int n_coefficients, double x){
    double y = 0.0;

    int i;
    for (i = 0; i < n_coefficients; i++){
        y = y*x + coefficients[i];
    }

    return y;
}

/* The largest double value of 1.7976931348623157e+308 can be halved 2098 */
/* times until it is zero. */
#define AR_POLYNOMIAL_MAX_ITERATIONS 2100

int ar_polynomial_root(double *coefficients, int n_coefficients, double x0, double x1, double tolerance, double *root){
    double dx = 0.5*(x1 - x0);

    x0 += dx;
    x1 = x0;

    int i;
    for (i = 0; i < AR_POLYNOMIAL_MAX_ITERATIONS; i++){
        dx *= 0.5;
        x0 -= dx;
        x1 += dx;

        double px0 = ar_polynomial_eval(coefficients, n_coefficients, x0);
        double px1 = ar_polynomial_eval(coefficients, n_coefficients, x1);

        if (px0*px1 < 0.0) break;
    }

    if (i == AR_POLYNOMIAL_MAX_ITERATIONS){
        return AR_POLYNOMIAL_ERROR_BISECTION_INTERVAL_NOT_FOUND;
    }

    for (i = 0; i < AR_POLYNOMIAL_MAX_ITERATIONS; i++){
        double middle = 0.5*(x0 + x1);

        double pmiddle = ar_polynomial_eval(coefficients, n_coefficients, middle);

        if (-tolerance < pmiddle && pmiddle < tolerance){
            *root = middle;
            return 0;
        }

        double px0 = ar_polynomial_eval(coefficients, n_coefficients, x0);

        if (px0*pmiddle > 0){
            x0 = middle;
        }else{
            x1 = middle;
        }
    }

    *root = x0;

    return AR_POLYNOMIAL_ERROR_DID_NOT_CONVERGE;
}
