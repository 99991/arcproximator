#include "ar_durand_kerner_roots.h"

AR_INLINE vec2 cmul(vec2 a, vec2 b){
    return v2(a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x);
}

AR_INLINE vec2 cdiv(vec2 a, vec2 b){
    return v2smul(1.0/v2len2(b), v2(v2dot(a, b), -v2det(a, b)));
}

vec2 ar_polynomial_eval_complex(const double *coeffs, int n, vec2 x){
    vec2 y = v2(0.0, 0.0);

    int i;
    for (i = 0; i < n; i++){
        y = v2add(cmul(x, y), v2(coeffs[i], 0.0));
    }

    return y;
}

int ar_durand_kerner_roots(
    const double *coeffs,
    int n_coeffs,
    vec2 *roots,
    int max_iterations,
    double tolerance
){
    int n_roots = n_coeffs - 1;

    int i;

    roots[0] = v2(1.0, 0.0);
    for (i = 1; i < n_roots; i++){
        roots[i] = cmul(roots[i - 1], v2(0.4, 0.8));
    }

    int iteration;
    for (iteration = 0; iteration < max_iterations; iteration++){
        vec2 new_roots[n_roots];
        vec2 max_change = v2(0.0, 0.0);

        for (i = 0; i < n_roots; i++){
            vec2 root = roots[i];

            vec2 change = ar_polynomial_eval_complex(coeffs, n_coeffs, root);

            vec2 p = v2(1.0, 0.0);
            int j;
            for (j = 0; j < n_roots; j++){
                if (i != j){
                    p = cmul(p, v2sub(root, roots[j]));
                }
            }

            p = v2smul(coeffs[0], p);

            change = cdiv(change, p);
            new_roots[i] = v2sub(root, change);

            if (v2len2(max_change) < v2len2(change)){
                max_change = change;
            }

        }

        for (i = 0; i < n_roots; i++){
            roots[i] = new_roots[i];
        }

        if (v2len2(max_change) < tolerance) return iteration;
    }

    return -1;
}
