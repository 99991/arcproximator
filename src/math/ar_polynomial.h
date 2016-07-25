#ifndef AR_POLYNOMIAL_INCLUDED
#define AR_POLYNOMIAL_INCLUDED

#define AR_POLYNOMIAL_ERROR_BISECTION_INTERVAL_NOT_FOUND -1
#define AR_POLYNOMIAL_ERROR_DID_NOT_CONVERGE             -2

double ar_polynomial_eval(double *coefficients, int n_coefficients, double x);
int ar_polynomial_root(double *coefficients, int n_coefficients, double x0, double x1, double tolerance, double *root);

#endif
