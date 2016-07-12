#ifndef AR_DURAND_KERNER_INCLUDED
#define AR_DURAND_KERNER_INCLUDED

#include "vec2.h"

vec2 ar_polynomial_eval_complex(const double *coeffs, int n, vec2 x);

int ar_durand_kerner_roots(
    const double *unscaled_coeffs,
    int n_coeffs,
    vec2 *roots,
    int max_iterations,
    double tolerance
);

#endif
