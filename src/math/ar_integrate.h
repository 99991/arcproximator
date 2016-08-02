#ifndef AR_INTEGRATE_INCLUDED
#define AR_INTEGRATE_INCLUDED

/* error is bounded by max[a, b] |f''''(x)| * some constant */
/* so it doesn't necessarily have to be less than tolerance */
double integrate(double (*f)(double, void*), double a, double c, double tolerance, void *data);

#endif
