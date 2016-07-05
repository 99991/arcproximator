#include "../durand_kerner.h"
#include <assert.h>
#include <stdio.h>

vec2 find_closest(const vec2 *values, int n, vec2 value){
    vec2 closest = values[0];
    for (int i = 1; i < n; i++){
        if (v2dist2(value, closest) > v2dist2(value, values[i])){
            closest = values[i];
        }
    }
    return closest;
}

void test_coefficients(const double *coeffs, int n_coeffs, const vec2 *correct_roots, int max_iterations, double convergence_tolerance, double roots_tolerance){
    int n_roots = n_coeffs - 1;
    vec2 roots[n_roots];

    int iterations = durand_kerner(coeffs, n_coeffs, roots, max_iterations, convergence_tolerance);

    if (iterations < 0){
        printf("ERROR: did not converge\n");
    }

    assert(iterations != -1);

    for (int i = 0; i < n_roots; i++){
        vec2 root = roots[i];
        vec2 closest = find_closest(correct_roots, n_roots, root);
        if (v2dist(closest, root) >= roots_tolerance){
            printf("ERROR: %e\n", v2dist(closest, root));
            assert(v2dist(closest, root) < roots_tolerance);
        }
    }

    printf("test passed\n");
}

int main(){
    double coeffs0[4] = {1, 0, -2, 2};

    vec2 correct_roots0[3] = {
        v2(-1.769292354238631415240409, 0.0),
        v2(0.8846461771193157076202047, +0.5897428050222055016472807),
        v2(0.8846461771193157076202047, -0.5897428050222055016472807),
    };

    double coeffs1[9] = {1, -12, 62, -180, 321, -360, 248, -96, 16};

    vec2 correct_roots1[8] = {
        v2(1, 0.0), v2(1, 0.0), v2(1, 0.0), v2(1, 0.0),
        v2(2, 0.0), v2(2, 0.0), v2(2, 0.0), v2(2, 0.0),
    };

    double coeffs2[5] = {1, 0, -1, 0, -11.0/36.0};

    vec2 correct_roots2[4] = {
        v2(+sqrt(0.5 + sqrt(5)/3), 0.0),
        v2(-sqrt(0.5 + sqrt(5)/3), 0.0),
        v2(0.0, +sqrt(sqrt(20.0/36.0) - 0.5)),
        v2(0.0, -sqrt(sqrt(20.0/36.0) - 0.5)),
    };

    double coeffs3[6] = {1, -15, 85, -225, 274, -120};

    vec2 correct_roots3[5] = {
        v2(1.0, 0.0),
        v2(2.0, 0.0),
        v2(3.0, 0.0),
        v2(4.0, 0.0),
        v2(5.0, 0.0),
    };

    test_coefficients(coeffs0, 4, correct_roots0, 10, 1e-20, 1e-20);
    test_coefficients(coeffs1, 9, correct_roots1, 100, 1e-7, 1e-3);
    test_coefficients(coeffs2, 5, correct_roots2, 10, 1e-15, 1e-15);
    test_coefficients(coeffs3, 6, correct_roots3, 30, 1e-10, 1e-5);

    return 0;
}
