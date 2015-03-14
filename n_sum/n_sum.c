#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char* argv[]) {
    assert(argc > 1);
    long n = atol(argv[1]);
    assert(n > 0);
    int rank, size;
    
    MPI_Status status;
    MPI_Request request;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    
    long step = n / size;
    long first_number = step * rank + 1;
    long last_number = rank == size - 1 ? n : step * (rank + 1);
    printf("from %ld to %ld\n", first_number, last_number);
    MPI_Barrier(MPI_COMM_WORLD);
    
    long local_sum = 0, i;
    for (i = first_number; i <= last_number; i++)
        local_sum += i;
    long global_sum = 0;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0) 
        printf("sum is: %ld\n", global_sum);
    MPI_Finalize();
}