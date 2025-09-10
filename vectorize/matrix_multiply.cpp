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


void matmat_unrolled(int n, double* __restrict__ A, double* __restrict__ B, double* __restrict__ C, int n_iter)
{
    double val;
    for (int iter = 0; iter < n_iter; iter++)
    {
        for (int i = 0; i < n; i++)
        {
            for (int k = 0; k < n; k++)
            {
                C[i*n+k] = 0;
            }

            for (int j = 0; j < n; j++)
            {
                val = A[i*n+j];
                for (int k = 0; k < n; k += 4)
                {
                    C[i*n+k] += val * B[j*n+k];
                    C[i*n+k+1] += val * B[j*n+k+1];
                    C[i*n+k+2] += val * B[j*n+k+2];
                    C[i*n+k+3] += val * B[j*n+k+3];
                }
            }
        }
    }
}



void inner_matmat(int n, double* __restrict__ A, double* __restrict__ B, double* __restrict__ C,
        int start_i, int start_j,int start_k, int n_orig)
{
    double val;
    int end_i = start_i + n;
    int end_j = start_j + n;
    int end_k = start_k + n;

    for (int i = start_i; i < end_i; i++)
    {
        for (int j = start_j; j < end_j; j++)
        {
            // This is the correct way to stripe matrix multiplication
            val = A[i*n_orig+j];
            for (int k = start_k; k < end_k; k++)
            {
                C[i*n_orig+k] += val * B[j*n_orig+k];
            }


            // Don't Do This!  SLOW : Cannot vectorize
            //val = 0;
            //for (int k = start_k; k < end_k; k++)
            //{
                //val += A[i*n_orig+k] * B[k*n_orig+j];
            //}
            //C[i*n_orig+j] += val;


            // Don't Do This!  SLOW : Out of order accesses!
            /*val = B[j*n_orig+i];
            for (int k = start_k; k < end_k; k++)
            {
                C[k*n_orig+i] += A[k*n_orig+j] * val;
            }*/

        }
    }
}

void matmat_split(int step, int n, double* __restrict__ A, double* __restrict__ B, double* __restrict__ C, int n_iter)
{

    for (int iter = 0; iter < n_iter; iter++)
    {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                C[i*n+j] = 0;

        for (int i = 0; i < n; i += step)
        {
            for (int j = 0; j < n; j+= step)
            {
                for (int k = 0; k < n; k += step)
                {
                    inner_matmat(step, A, B, C, i, j, k, n);
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

    if (argc < 2)
    {
        printf("Need Matrix Dimemsion n and step size k passed as Command Line Arguments (e.g. ./matmat 8 2)\n");
        return 0;
    }

    int n = atoi(argv[1]);
    int step = atoi(argv[2]);

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


    // Comparisons
    matmat(n, A, B, C, 2);
    matmat_unrolled(n, A, B, C_new, 2);
    for (int i = 0; i < n*n; i++)
        if (fabs(C[i] - C_new[i]) > 1e-10)
        {
            printf("Different Answers (Unrolled)! idx %d, %e vs %e\n", i, C[i], C_new[i]);
            return 0;
        }

    matmat_split(step, n, A, B, C_new, 1);
    for (int i = 0; i < n*n; i++)
        if (fabs(C[i] - C_new[i]) > 1e-10)
        {
            printf("Different Answers (Split)! idx %d, %e vs %e\n", i, C[i], C_new[i]);
            return 0;
        }

    // Warm-Up 
    matmat(n, A, B, C, n_iter);

    start = get_time();
    matmat(n, A, B, C, n_iter);
    end = get_time();
    printf("N %d, Time Per MatMat %e\n", n, (end - start)/n_iter);


    // Warm-Up 
    matmat_unrolled(n, A, B, C_new, 2);

    start = get_time();
    matmat_unrolled(n, A, B, C_new, 2);
    end = get_time();
    printf("N %d, Time Per MatMat Unrolled %e\n", n, (end - start) / n_iter);


    // Warm-Up 
    matmat_split(step, n, A, B, C, n_iter);

    start = get_time();
    matmat_split(step, n, A, B, C, n_iter);
    end = get_time();
    printf("N %d, Time Per MatMat Split %e\n", n, (end - start) / n_iter);

    free(A);
    free(B);
    free(C);
        

    return 0;
}

