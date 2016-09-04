#include "../integrate.h"

#include <math.h>

double pi = 3.14159265358979323846264;

double f(double x, void *unused){
    return exp(-x*x);
}

double F(double x){
    return 0.5*sqrt(pi)*erf(x);
}

double g(double x, void *unused){
    return atan(x);
}

double G(double x){
    return x*atan(x) - 0.5*log(x*x + 1.0);
}

double h(double x, void *unused){
    return x*log(x) - x;
}

double H(double x){
    return 0.25*x*x*(2.0*log(x) - 3.0);
}

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

void test_function(double (*f)(void*, double), double (*F)(double), double a, double b, double tolerance){
    double c = integrate(f, a, b, tolerance, NULL);
    double d = F(b) - F(a);
    double error = fabs(c - d);
    if (error > tolerance){
        printf("WARNING: error exceeds tolerance:\n");
    }
    printf("difference to exact solution is: %e\n", error);
}

int main(){
    test_function(f, F, -1.0, +1.0, 1e-10);
    test_function(g, G, 0.0, 1.0, 1e-10);
    test_function(h, H, 0.1, 1.0, 1e-10);
    test_function(cos, sin, 0.0, 1.0, 1e-10);

    return 0;
}
