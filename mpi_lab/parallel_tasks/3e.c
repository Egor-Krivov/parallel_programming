#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../conventions.h"

enum calculation_type get_calculation_type() {
    return VERTICAL;
}

int get_parallelism() {
    return 5;
}


double *calculate_thread(int thread) {
    int external_lim = get_external_limit();
    int internal_lim = get_internal_limit();

    int parallelism = get_parallelism();
    assert(thread < get_parallelism());

    int rows = external_lim / parallelism + 1;
    double *data = (double *) malloc(sizeof(double) * internal_lim * rows);
    int i, j, k;

    for (i = thread, k = 0; i < external_lim; i += parallelism, k++)
        for (j = 0; j < internal_lim; j++)
            data[internal_lim * k + j] = 0;

    double val;
    for (i = thread + parallelism, k = 1; i < external_lim; i += parallelism, k++)
        for (j = 0; j < internal_lim - 3; j++) {
            val = 10 * (j+3) + (i-5);
            val = sin(MUL * val);
            //data[k*internal_lim + j] = val[j+3][i-5]
            data[k*internal_lim + j] = val * 3;
            //sin(MUL*data[(k-1)*internal_lim + j + 2]);
        }

    return data;
}
