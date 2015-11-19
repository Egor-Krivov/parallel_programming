#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#define ISIZE 10000
#define JSIZE 1000
#define MUL 0.000001

int main(int argc, char* argv[]) {
    int i, j;
    int results = 1;
    if (argc > 1)
        results = 0;
    FILE *ff = fopen("results.txt", "w");
    double val;
    for (i = 0; i < ISIZE; i++) {
        for (j = 0; j < JSIZE; j++) {
            val = 10 * i + j;
            val = sin(MUL * val);
	    if (results)
                fprintf(ff, "%lf ", val);
        }
        if (results)
            fprintf(ff, "\n");
    }
    fclose(ff);
    return 0;
}
