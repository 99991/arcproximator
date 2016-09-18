#include "../src/math/ar_durand_kerner_roots.c"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

vec2 find_closest(const vec2 *values, int n, vec2 value){
    vec2 closest = values[0];
    for (int i = 1; i < n; i++){
        if (v2dist2(value, closest) > v2dist2(value, values[i])){
            closest = values[i];
        }
    }
    return closest;
}

void test_coefficients(const double *coeffs, int n_coeffs, const vec2 *expected_roots, int max_iterations, double convergence_tolerance, double roots_tolerance){
    int n_roots = n_coeffs - 1;
    vec2 roots[n_roots];

    int iterations = ar_durand_kerner_roots(coeffs, n_coeffs, roots, 1000, convergence_tolerance);

    if (iterations < 0 || iterations > max_iterations){
        printf("ERROR: did not converge after %i iterations\n", iterations);
        assert(iterations <= max_iterations);
    }

    int i, j;
    for (i = 0; i < n_roots; i++){
        vec2 root = roots[i];
        vec2 closest = find_closest(expected_roots, n_roots, root);
        if (v2dist(closest, root) >= roots_tolerance){
            printf("ERROR: %e\n", v2dist(closest, root));
            printf("coefficients:\n");
            for (j = 0; j < n_coeffs; j++){
                printf("%i: %f\n", j, coeffs[j]);
            }
            printf("expected roots:\n");
            for (j = 0; j < n_roots; j++){
                vec2 root = expected_roots[i];
                printf("%i: %f %f\n", j, root.x, root.y);
            }
            assert(v2dist(closest, root) < roots_tolerance);
        }
    }
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

    double coeffs4[3] = {-666, -666, 3996};

    vec2 correct_roots4[2] = {
        v2(2.0, 0.0),
        v2(-3.0, 0.0),
    };

    test_coefficients(coeffs0, 4, correct_roots0, 10, 1e-20, 1e-20);
    test_coefficients(coeffs1, 9, correct_roots1, 100, 1e-7, 1e-3);
    test_coefficients(coeffs2, 5, correct_roots2, 10, 1e-15, 1e-15);
    test_coefficients(coeffs3, 6, correct_roots3, 30, 1e-10, 1e-5);
    test_coefficients(coeffs4, 3, correct_roots4, 10, 1e-10, 1e-10);

    int i;

    for (i = 0; i < 1000; i++){
        double root0 = rand()%1000 - 500;
        double root1 = rand()%1000 - 500;
        double factor = rand()%1000 - 500;

        double coeffs[3] = {
            factor,
            -factor*(root0 + root1),
            factor*root0*root1,
        };

        vec2 correct_roots[2] = {
            v2(root0, 0.0),
            v2(root1, 0.0),
        };

        test_coefficients(coeffs, 3, correct_roots, 40, 1e-10, 1e-5);
    }

    for (i = 0; i < 10000; i++){
        double root0 = rand()%1000 - 500;
        double root1 = rand()%1000 - 500;
        double root2 = rand()%1000 - 500;
        double factor = rand()%1000 - 500;

        double coeffs[4] = {
            factor,
            -factor*(root0 + root1 + root2),
            factor*(root0*root1 + root1*root2 + root2*root0),
            -factor*root0*root1*root2,
        };

        vec2 correct_roots[3] = {
            v2(root0, 0.0),
            v2(root1, 0.0),
            v2(root2, 0.0),
        };

        test_coefficients(coeffs, 4, correct_roots, 1000, 1e-10, 1e-4);
    }

    return 0;
}
