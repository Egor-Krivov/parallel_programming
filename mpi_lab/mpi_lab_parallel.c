#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#define ISIZE 1000
#define JSIZE 10000
#define MUL 0.0000001

int main(int argc, char* argv[]) {

    int rank, size;
    MPI_Status status;
    MPI_Request request;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    
    int step = ISIZE / size;
    int first_number = step * rank;
    int last_number = rank == size - 1 ? ISIZE : step * (rank + 1);
    int rows = last_number - first_number;
    
    int i, j;
    double *data;
    double val;
    data = (double *) malloc(sizeof(double) * JSIZE * rows);
    for (i = 0; i < rows; i++)
        for (j = 0; j < JSIZE; j++) {
            val = 10 * (i + first_number) + j;
            val = sin(MUL * val);
            data[i * JSIZE + j] = val;
        }
    
    FILE *ff;
    
    if (rank != 0) {
        int source = rank - 1;
        MPI_Recv(0, 0, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
        ff = fopen("results.txt", "a");
    }
    else {
        ff = fopen("results.txt", "w");
    }
    for (i = 0; i < rows; i++) {
        for (j = 0; j < JSIZE; j++) {
            fprintf(ff, "%lf ", data[i * JSIZE + j]);
        }
        fprintf(ff, "\n");
    }
    fclose(ff);
    if (rank != size - 1) {
        int dest = rank + 1;
        MPI_Send(0, 0, MPI_INT, dest, 0, MPI_COMM_WORLD);
    }
    free(data);
    MPI_Finalize();
}