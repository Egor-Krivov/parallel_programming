#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#define MAX_X 1.0
#define MAX_T 1.0
#define PI 3.141592
#define E 2.718282

#define P 1.0

void print_line(double *line, int n) {
    int i;
    for (i = 0; i < n; i++) {
        printf("%lf ", line[i]);
    }
    putchar('\n');
}

void print_grid(double **grid, int num_x, int num_t) {
    int j;
    for (; num_t >= 0; num_t--) {
        for (j = 0; j <= num_x; j++) {
            printf("%lf ", grid[num_t][j]);
        }
        putchar('\n');
    }
}

double x_border_value(double x) {
    x = PI * pow(x / MAX_X, 4.0);
    x = fabs(sin(x));
    x = pow(x, 3.0);
    //x = sin(2 * PI * x / MAX_X);
    return x;
}

double t_border_value(double t) {
    t *= pow(E, -P * t);
    return t;
}

double next_value_triangle(double left_value, double central_value, double f_value, double h, double tau) {
    double value = (left_value - central_value) / h + f_value;
    value *= tau;
    value += central_value;
    return value;
}

double f(double x, double t) {
    return 0;
}
int main(int argc, char* argv[]) {
    assert(argc > 2);
    int num_x = atoi(argv[1]);
    int num_t = atof(argv[2]);
    assert(num_x > 2 && num_t > 2);
    double h = MAX_X / num_x;
    double tau = MAX_T / num_t;
    
    int should_print_grid = 1;
    if (argc > 3 && argv[3][0] == '0') {
        should_print_grid = 0;
    }
    
    int rank, size;
    MPI_Status status;
    MPI_Request request;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    
    assert(size > 1);
    assert((num_x + 1) >= size);
    
    double start = MPI_Wtime();
    double elapsed_time;
    
    int columns_needed = (num_x + 1) / size;
    if (rank == (size - 1)) {
        columns_needed += (num_x + 1) % size;
    }
    int left_bound = rank * ((num_x + 1) / size);
    int right_bound = left_bound + columns_needed;
    
//     MPI_Barrier(MPI_COMM_WORLD);
//     printf("%d:%d:%d:%d\n", rank, columns_needed, left_bound, right_bound);
    
    int i;
    double **data = malloc(sizeof(double *) * (num_t + 1));
    for (i = 0; i <= num_t; i++) {
        data[i] = malloc(sizeof(double) * columns_needed);
    }
    
    //Calculating grid values
    int j;
    for (j = 0; j < columns_needed; j++) {
        data[0][j] = x_border_value(h * (j + left_bound));
    }
    if (rank != (size - 1)) {
        MPI_Send(&data[0][columns_needed - 1], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
    }
//     if (rank == 2) {
//         print_line(data[0], columns_needed);
//     }
    
    double left_value;
    double f_value;
    for (i = 1; i <= num_t; i++) {
        for (j = 0; j < columns_needed; j++) {
            if (j == 0) {
                if (rank == 0) {
                    data[i][j] = t_border_value(i * tau);
                }
                else {
                    f_value = f((left_bound + j) * h, i * tau);
                    MPI_Recv(&left_value, 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &status);
                    data[i][j] = next_value_triangle(left_value, data[i - 1][j], f_value, h, tau);
                }
            }
            else {
                f_value = f((left_bound + j) * h, i * tau);
                data[i][j] = next_value_triangle(data[i - 1][j - 1], data[i - 1][j], f_value, h, tau);
            }
        }
        if (i != num_t && rank != (size - 1)) {
            MPI_Send(&data[i][columns_needed - 1], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
        }
    }
    //Gathering results in the first process
    double time_calculating = MPI_Wtime() - start;
//     MPI_Barrier(MPI_COMM_WORLD);
//     for (i = 0; i <= num_t; i++) {
//         print_line(data[i], columns_needed);
//     }
    
    int gatherer = 0;
    double **grid = malloc(sizeof(double *) * (num_t + 1));
    if (rank == gatherer) {
        for (i = 0; i <= num_t; i++) {
            grid[i] = malloc(sizeof(double) * num_x);
        }
        int sender;
        for (i = 0; i <= num_t; i++) {
            for (j = 0; j < columns_needed; j++) {
                grid[i][j] = data[i][j];
            }
        }
        
        for (sender = 1; sender < size; sender++) {
            int sender_columns_count = (num_x + 1) / size;
            if (sender == (size - 1)) {
                sender_columns_count += (num_x + 1) % size;
            }
            int senders_left_bound = ((num_x + 1) / size) * sender;
            //printf("sender %d", sender);
            for (i = 0; i <= num_t; i++) {
                MPI_Recv(&grid[i][senders_left_bound], sender_columns_count, MPI_DOUBLE, sender, 0, MPI_COMM_WORLD, &status);
            }
        }
        //Grid is ready
        elapsed_time = MPI_Wtime() - start;
        if (should_print_grid) {
            print_grid(grid, num_x, num_t);
        }
        printf("\nElapsed time: %.2fs\nIncluding calculating time %.2fs\n", elapsed_time, time_calculating);
        
        for (i = 0; i <= num_t; i++) {
            free(grid[i]);
        }
        free(grid);
    }
    else {
        //printf("%d is sending ", rank);
        for (i = 0; i <= num_t; i++) {
            MPI_Send(data[i], columns_needed, MPI_DOUBLE, gatherer, 0, MPI_COMM_WORLD);
        }
    }
    for (i = 0; i < num_t; i++) {
        free(data[i]);
    }
    free(data);
    MPI_Finalize();
}