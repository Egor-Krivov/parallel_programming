#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ISIZE 3000
#define JSIZE 2000
#define MUL 0.00001

double a[ISIZE][JSIZE];

int main(int argc, char* argv[]) {
    FILE *ff = 0;
    int results;
    if (argc == 1) {
        results = 1;
        ff = fopen("results.txt", "w");
    }
    else
        results = 0;

    int i, j;

    for (i=0; i<ISIZE; i++){
        for (j=0; j<JSIZE; j++){
            a[i][j] = 10*i +j;
        }
    }

    for (i=3; i<ISIZE; i++){
        for (j = 0; j < JSIZE-2; j++){
            a[i][j] = sin(MUL*a[i-3][j+2]); 
        }
    }
    
    if (results) {
        for (i=0; i < ISIZE; i++){
            for (j=0; j < JSIZE; j++){
                fprintf(ff,"%f ",a[i][j]);
            }
            fprintf(ff,"\n");
        }
        fclose(ff);
    }
    
    return 0;
}