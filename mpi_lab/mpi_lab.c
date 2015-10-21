#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#define ISIZE 1000
#define JSIZE 10000
#define MUL 0.0000001

int main(int argc, char* argv[]) {
    int i, j;
    FILE *ff = fopen("results.txt", "w");
    double val;
    for (i = 0; i < ISIZE; i++)
        for (j = 0; j < JSIZE; j++) {
            val = 10 * i + j;
            fprintf(ff, "%lf ", sin(MUL * val));
        }
    fclose(ff);
    return 0;
}