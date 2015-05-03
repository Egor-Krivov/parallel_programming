#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char* argv[]) {
    int rank, size;
    assert(argc > 1);
    int n = atoi(argv[1]);
    MPI_Status status;
    MPI_Request request;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);


    int *data =(int *) malloc(n * sizeof(int));
    if (rank == 0) {
        MPI_Send(data, n, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(data, n, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
    }
    else if (rank == 1) {
        MPI_Recv(data, n, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Send(data, n, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    int i;
    int count = 1000;
    double total_time = 0.0;
    for (i = 0; i < count; i++) {
        if (rank == 0) {
            double time = MPI_Wtime();
            MPI_Send(data, n, MPI_INT, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(data, n, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
            time = MPI_Wtime() - time;
            total_time += time;
        }
        else if (rank == 1) {
            MPI_Recv(data, n, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            MPI_Send(data, n, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }
    free(data);
    if (rank == 0)
        printf("%e", total_time / (2 * count));

    MPI_Finalize();
}
