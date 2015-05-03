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

double next_value(double left_value, double central_value, double right_value, double f_value, double h, double tau) {
    double alpha = tau / h;
    double value = right_value - 2 * central_value + left_value;
    value *= alpha;
    value += left_value - right_value;
    value *= alpha / 2;
    value += central_value + f_value * tau;
    return value;
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
    
    int rank, size;
    MPI_Status status;
    MPI_Request request;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    
    assert(size > 1);
    
    double start = MPI_Wtime();
    double elapsed_time;
    
    int destination = (rank + 1) % size;
    int source = rank - 1;
    if (source == -1) {
        source = size - 1;
    }
    
    int lines_needed = (num_t + 1) / size;
    if ((num_t + 1) % size > rank) {
        lines_needed++;
    }
    
    //MPI_Barrier(MPI_COMM_WORLD);
    //printf("%d:%d\n", rank, lines_needed);
    
    int i;
    double **data = malloc(sizeof(double *) * (lines_needed));
    
    for (i = 0; i < lines_needed; i++) {
        data[i] = malloc(sizeof(double) * (num_x + 1));
    }
    
    
    //Calculating grid values
    int j;
    i = rank;
    int line = 0;
    double values[3];
    double f_value;
    if (i == 0) {
        for (j = 0; j <= num_x; j++) {
            data[line][j] = x_border_value(j * h);
            MPI_Send(&data[line][j], 1, MPI_DOUBLE, destination, 0, MPI_COMM_WORLD);
        }
        i += size;
        line++;
    }
    for (; i <= num_t; i += size, line++) {
        data[line][0] = t_border_value(i * tau);
        if (i != num_t) {
            MPI_Send(&data[line][0], 1, MPI_DOUBLE, destination, 0, MPI_COMM_WORLD);
        }
        MPI_Recv(&values[0], 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&values[1], 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, &status);
        for (j = 1; j <= num_x; j++) {
            f_value = f(j * h, i * tau);
            if (j == num_x) {
                data[line][j] = next_value_triangle(values[(j - 1) % 3], values[j % 3], f_value, h, tau);
            }
            else {
                MPI_Recv(&values[(j + 1) % 3], 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, &status);
                data[line][j] = next_value(values[(j - 1) % 3], values[j % 3], values[(j + 1) % 3], f_value, h, tau);
            }
            if (i != num_t) {
                MPI_Send(&data[line][j], 1, MPI_DOUBLE, destination, 0, MPI_COMM_WORLD);
            }
        }
    }
    //Gathering results in the last process
    
    int gatherer = size - 1;
    if (rank == gatherer) {
        double **grid = malloc(sizeof(double *) * (num_t + 1));
        for (i = 0, line = 0; i <= num_t; i++) {
            if ((i % size) == gatherer) {
                grid[i] = data[line++];
            }
            else {
                grid[i] = malloc(sizeof(double) * (num_x + 1));
                MPI_Recv(grid[i], num_x + 1, MPI_DOUBLE, i % size, 0, MPI_COMM_WORLD, &status);
            }
        }
        //Grid is ready
        elapsed_time = MPI_Wtime() - start;
        for (i = num_t; i >= 0; i--) {
            for (j = 0; j <= num_x; j++) {
                printf("%lf ", grid[i][j]);
            }
            putchar('\n');
        }
        printf("\nElapsed time: %.2fs\n", elapsed_time);
    }
    else {
        for (line = 0; line < lines_needed; line++) {
            MPI_Send(data[line], num_x + 1, MPI_DOUBLE, gatherer, 0, MPI_COMM_WORLD);
            free(data[line]);
        }
    }
    free(data);
    MPI_Finalize();
}