#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../conventions.h"

enum calculation_type get_calculation_type() {
    return VERTICAL;
}

int get_parallelism() {
    return 3;
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
            data[internal_lim * k + j] = 10 * j + i;

    for (i = thread + parallelism, k = 1; i < external_lim-1; i += parallelism, k++)
        for (j = 1; j < internal_lim; j++)
            data[k*internal_lim + j] = sin(MUL*data[(k-1)*internal_lim + j - 1]);

    return data;
}
