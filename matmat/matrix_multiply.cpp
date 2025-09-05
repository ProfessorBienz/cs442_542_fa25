#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <cstring>
#include "../timer.h"

// To compile with and without vectorization (in gcc):
// gcc -o <executable_name> <file_name> -O1     <--- no vectorization
// Flag to vectorize : -ftree-vectorize  
// Flag needed for vectorization of X86 processors : -msse -msse2
// Flag needed for vectorization of PowerPC platforms : -maltivec
// Other optional flags (floating point reductions) : -ffast-math -fassociative-math
//
// To see what the compiler vectorizes : -fopt-info-vec (or -fopt-info-vec-optimized)
// To see what the compiler is not able to vectorize : -fopt-info-vec-missed


// Matrix-Matrix Multiplication of Doubles (Double Pointer)
void matmat(int n, double* A, double*  B, double* C, int n_iter)
{
    memset(C, 0, n * n * sizeof(double));
    
    for (int iter = 0; iter < n_iter; iter++) {
        for (int i = 0; i < n; i++) 
        {
            for (int j = 0; j < n; j++) 
            {
                for (int k = 0; k < n; k++)
                {
                    C[i*n + j] += A[i*n + k] * B[k*n + j];
                }
            }
        }
    }
}


int main(int argc, char* argv[])
{

    double start, end;
    int n_access = 1000000000;

    if (argc < 1)
    {
        printf("Need Matrix Dimemsion n passed as Command Line Arguments (e.g. ./matmat 8)\n");
        return 0;
    }

    int n = atoi(argv[1]);

    int n_iter = (n_access / (n*n*n)) + 1;

    double* A = (double*)malloc(n*n*sizeof(double));
    double* B = (double*)malloc(n*n*sizeof(double));
    double* C = (double*)malloc(n*n*sizeof(double));
    double* C_new = (double*)malloc(n*n*sizeof(double));

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            A[i*n+j] = 1.0/(i+1);
            B[i*n+j] = 1.0;
        }
    }

    // Warm-Up 
    matmat(n, A, B, C, 1);

    start = get_time();
    matmat(n, A, B, C, n_iter);
    end = get_time();
    printf("N %d, Time Per MatMat %e\n", n, (end - start)/n_iter);


    free(A);
    free(B);
    free(C);
        

    return 0;
}
