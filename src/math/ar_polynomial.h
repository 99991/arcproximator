#ifndef AR_POLYNOMIAL_INCLUDED
#define AR_POLYNOMIAL_INCLUDED

double ar_polynomial_eval(double *coefficients, int n_coefficients, double x);
double ar_polynomial_root(double *coefficients, int n_coefficients, double x0, double x1, double tolerance);

#endif
