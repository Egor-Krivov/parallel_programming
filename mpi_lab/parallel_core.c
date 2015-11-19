#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <assert.h>
#include "conventions.h"


int get_internal_limit() {
    if (get_calculation_type() == HORIZONTAL)
        return JSIZE;
    else
        return ISIZE;
}


int get_external_limit() {
    if (get_calculation_type() == HORIZONTAL)
        return ISIZE;
    else
        return JSIZE;
}


int main(int argc, char* argv[]) {

    int rank, size;
    int print_results;
    if (argc > 1)
        print_results = 0;
    else
        print_results = 1;

    MPI_Status status;
    MPI_Request request;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    // Choose thread count
    int parallelism = get_parallelism();
    if (rank >= parallelism) {
        // There is no point in additional threads
        MPI_Finalize();
        return 0;
    }
    size = size > parallelism ? parallelism : size;
    printf("parallelism is %d\n", parallelism);
    fflush(stdout);

    int internal_limit = get_internal_limit();
    int external_limit = get_external_limit();

    int i, j, k;
    int lines_per_thread = external_limit / parallelism + 1;
    double *periodic_lines[parallelism];


    double t1 = MPI_Wtime();
    for (i = rank; i < parallelism; i += size) {
        periodic_lines[i] = calculate_thread(i);
    }

    double t2 = MPI_Wtime();

    printf("finished calculating %d\n", rank);
    fflush(stdout);

    if (rank) {
        // Send data to the leader
        int dest = 0;
        for (i = rank; i < parallelism; i += size) {
            printf("Going to send from %d packet %d\n", rank, i);
            fflush(stdout);
            MPI_Send(periodic_lines[i], internal_limit * lines_per_thread, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
            printf("Sent from %d packet %d\n", rank, i);
            fflush(stdout);
        }
    }
    // Leader receives data
    else {
        int source;
        for (i = 0; i < parallelism; i++) {
            source = i % size;
            if (source) {
                printf("Going to receive from %d thread packet %d\n", source, i);
                fflush(stdout);
                periodic_lines[i] = (double *) malloc(sizeof(double) * internal_limit * lines_per_thread);
                MPI_Recv(periodic_lines[i], internal_limit * lines_per_thread, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, &status);
            }
        }
        // We have data. Now printing
        printf("First thread calculation time is %lf\n", t2 - t1);
        if (print_results) {
            FILE *ff;
            ff = fopen("results_parallel.txt", "w");
            double *line;
            int line_period;
            if (get_calculation_type() == HORIZONTAL) {
                for (i = 0; i < external_limit; i++) {
                    line_period = i % parallelism;
                    line = &periodic_lines[line_period][internal_limit * (i / parallelism)];
                    for (j = 0; j < internal_limit; j++) {
                        fprintf(ff, "%lf ", line[j]);
                    }
                    fprintf(ff, "\n");
                }
            }
            else {
                for (i = 0; i < internal_limit; i++) {
                    for (j = 0; j < external_limit; j++) {
                        line_period = j % parallelism;
                        line = &periodic_lines[line_period][internal_limit * (j / parallelism)];
                        fprintf(ff, "%lf ", line[i]);
                    }
                    fprintf(ff, "\n");
                }
            }
            fclose(ff);
        }
    }
    MPI_Finalize();
    return 0;
}
