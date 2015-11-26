#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <mpi.h>

#include "../conventions.h"

enum calculation_type get_calculation_type() {
    return HORIZONTAL;
}

int get_parallelism() {
    int size;
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    return size;
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
            data[internal_lim * k + j] = 10 * i + j;

    double val;
    for (i = thread, k = 0; i < external_lim-3; i += parallelism, k++)
        for (j = 2; j < internal_lim; j++) {
            val = 10 * (i + 3) + j - 2;
            data[k*internal_lim + j] = sin(MUL*val);
        }

    return data;
}
