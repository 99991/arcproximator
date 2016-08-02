#include "ar_integrate.h"

static double estimate_area_simpson_rule(double fa, double fb, double fc, double a, double c){
    return (1.0/6.0)*(c - a)*(fa + 4.0*fb + fc);
}

static double integrate_recursive(
    double (*f)(double, void*),
    double a,
    double c,
    double e,
    double fa,
    double fc,
    double fe,
    double area_ace,
    double tolerance,
    void *data
){
    double b = 0.5*(a + c);
    double d = 0.5*(c + e);

    double fb = f(b, data);
    double fd = f(d, data);

    double area_abc = estimate_area_simpson_rule(fa, fb, fc, a, c);
    double area_cde = estimate_area_simpson_rule(fc, fd, fe, c, e);

    double estimated_error = area_abc + area_cde - area_ace;

    if (estimated_error < -tolerance || tolerance < estimated_error){
        area_abc = integrate_recursive(f, a, b, c, fa, fb, fc, area_abc, tolerance, data);
        area_cde = integrate_recursive(f, c, d, e, fc, fd, fe, area_cde, tolerance, data);
    }

    return area_abc + area_cde;
}

double ar_integrate(double (*f)(double, void*), double a, double c, double tolerance, void *data){
    double b = 0.5*(a + c);

    double fa = f(a, data);
    double fb = f(b, data);
    double fc = f(c, data);

    double area = estimate_area_simpson_rule(fa, fb, fc, a, c);

    return integrate_recursive(f, a, b, c, fa, fb, fc, area, tolerance, data);
}
