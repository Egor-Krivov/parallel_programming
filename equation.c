#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>

#define MAX_X 1.0 //max x
#define MAX_T 1.0 //max t
#define PI 3.141592
#define E 2.718282

#define P 1.0

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

double next_value(double left_value, double central_value, double f_value, double h, double tau) {
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
    int i, j;
    
    clock_t start = clock();
    
    double **grid = malloc(sizeof(double *) * (num_t + 1));
    for (i = 0; i <= num_t; i++) {
        grid[i] = malloc(sizeof(double) * (num_x + 1));
    }

    for (i = 0; i <= num_t; i++) {
        grid[i][0] = t_border_value(tau * i);
    }
    
    for (i = 0; i <= num_x; i++) {
        grid[0][i] = x_border_value(h * i);
    }
    
    double f_value = 0.0;
    for (i = 1; i <= num_t; i++) {
        for (j = 1; j <= num_x; j++) {
            f_value = f(j * h, i * tau);
            grid[i][j] = next_value(grid[i - 1][j - 1], grid[i - 1][j], f_value, h, tau);
        }
    }
        
    clock_t end = clock();
    double elapsed_time = (end - start) / (double) CLOCKS_PER_SEC;

    for (i = num_t; i >= 0; i--) {
        for (j = 0; j <= num_x; j++) {
            printf("%lf ", grid[i][j]);
        }
        putchar('\n');
    }

    for (i = 0; i <= num_t; i++) {
        free(grid[i]);
    }
    free(grid);
    printf("\nElapsed time: %.2fs\n", elapsed_time);
}