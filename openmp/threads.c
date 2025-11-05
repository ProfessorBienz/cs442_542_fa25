#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "../timer.h"


void dot_product(double* A, double* B, double* C, int row, int col, int n)
{
    C[row*n+col] = 0;
    for (int i = 0; i < n; i++)
    {
        C[row*n+col] += A[row*n+i]*B[i*n+col];
    }
}



void matmult(double* A, double* B, double* C, int n)
{
#pragma omp parallel for collapse(1) shared(A, B, C)
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            dot_product(A, B, C, i, j, n);
        }
    }

}




void hello_world()
{
    int thread_id, num_threads;
#pragma omp parallel num_threads(16) 
{
    thread_id = omp_get_thread_num();
    usleep(10);
    num_threads = omp_get_num_threads();
    printf("Hello world from thread %d of %d\n", thread_id, num_threads);
}
}

// Out Of Order Matmult
void matmult_oor(double* A, double* B, double* C, int n)
{
    double val;
#pragma omp parallel for shared(A, B, C) private(val)
    for (int i = 0; i < n; i++)
    {
        for (int k = 0; k < n; k++)
            C[i*n+k] = 0;

        for (int j = 0; j < n; j++)
        {

            val = A[i*n+j];
            for (int k = 0; k < n; k++)
            {
                C[i*n+k] += val*B[j*n+k];
            }
        }
    }
}

double sum(double* A, int n)
{
    double global_sum = 0;
#pragma omp parallel
{
    double s = 0;
    int thread_id = omp_get_thread_num();
    int num_threads = omp_get_num_threads();
    int num_i = n / num_threads;
    int first_i = num_i * thread_id;

    for (int i = first_i; i < first_i + num_i; i++)
    {
        for (int j = 0; j < n; j++)
        {
            s += A[i*n+j];
        }
    }

#pragma omp critical
    {
        global_sum += s;
    }
}

    return global_sum;
}

int main(int argc, char* argv[])
{
    // Get size of vectors : command line argument
    if (argc == 1)
    {
        printf("Pass matrix dimension n as command line argument.\n");
        return 0;
    }

    int n = atoi(argv[1]);
    int n_iter = 100;
    double* A = (double*)malloc(n*n*sizeof(double));
    double* B = (double*)malloc(n*n*sizeof(double));
    double* C = (double*)malloc(n*n*sizeof(double));

    // Initialize vector values (less than one to prevent overflow)
    for (int i = 0; i < n*n; i++)
    {
        A[i] = (double)((double)rand() / RAND_MAX);
        B[i] = (double)((double)rand() / RAND_MAX);
    }

    // Calculate C = A*B
    /*matmult(A, B, C, n);
    double t0 = get_time();
    for (int i = 0; i < n_iter; i++)
        matmult(A, B, C, n);
    double tfinal = (get_time() - t0) / n_iter;
    printf("Time to multiply two %dx%d matrices: %e\n", n, n, tfinal);*/


    hello_world();

    /*
    matmult(A, B, C, n);
    printf("Sum %e\n", sum(C, n));

    matmult_oor(A, B, C, n);
    printf("Sum %e\n", sum(C, n));
    */
    free(A);
    free(B);
    free(C);
    return 0;
}






