#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../conventions.h"

enum calculation_type get_calculation_type() {
    return HORIZONTAL;
}

int get_parallelism() {
    return 3;
}


double *calculate_thread(int thread) {
    int external_lim, internal_lim;
    if (get_calculation_type() == HORIZONTAL) {
        external_lim = ISIZE;
        internal_lim = JSIZE;
    }
    else {
        internal_lim = ISIZE;
        external_lim = JSIZE;
    }
    
    int parallelism = get_parallelism();
    assert((thread < get_parallelism()));
    int rows = external_lim / parallelism + 1;
    double *data = (double *) malloc(sizeof(double) * internal_lim * rows);
    int i, j, k;
    for (i = thread, k = 0; i < external_lim; i += parallelism, k++)
        for (j = 0; j < internal_lim; j++)
            data[internal_lim * k + j] = 10 * i + j;

    for (i = thread + parallelism, k = 1; i < external_lim; i += parallelism, k++)
        for (j = 0; j < internal_lim - 2; j++)
            data[k*internal_lim + j] = sin(MUL*data[(k-1)*internal_lim + j + 2]);

    return data;
}