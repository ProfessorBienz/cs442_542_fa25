#include <stdlib.h>
#include <stdio.h>
#include <cmath>
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
// Test without the restrict variables
void matmat(int n, double* __restrict__ A, double* __restrict__ B, double* __restrict__ C, int n_iter)
{
    double val;
    for (int iter = 0; iter < n_iter; iter++)
    {
        for (int i = 0; i < n; i++)
        {
            for (int k = 0; k < n; k++)
                C[i*n+k] = 0;

            for (int j = 0; j < n; j++)
            {
                val = A[i*n+j];
                for (int k = 0; k < n; k++)
                {
                    C[i*n+k] += val * B[j*n+k];
                }
            }
        }
    }
}



// This program runs matrix matrix multiplication with double pointers
// Test vectorization improvements for both doubles and floats
// Try with and without the restrict variables
int main(int argc, char* argv[])
{

    double start, end;
    int n_access = 1000000000;

    if (argc < 1)
    {
        printf("Need Matrix Dimemsion n passed as Command Line Arguments (e.g. ./matmat 8 2)\n");
        return 0;
    }

    int n = atoi(argv[1]);

    int n_iter;
    if (n > 100) n_iter = 1;
    else
        n_iter = (n_access / (n*n*n));

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
    matmat(n, A, B, C, n_iter);

    start = get_time();
    matmat(n, A, B, C, n_iter);
    end = get_time();
    printf("N %d, Time Per MatMat %e\n", n, (end - start)/n_iter);



    free(A);
    free(B);
    free(C);
        

    return 0;
}

