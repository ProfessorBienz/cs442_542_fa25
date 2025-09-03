
#include <stdlib.h>
#include <stdio.h>

// Import timer.h (the other file I have uploaded) 
// as it has all of the timing methods
#include "timer.h"


// Vector norm method ... just for checking that results are consistent
double norm(double* vals, int n_vals)
{
    double sum = 0;
    for (int i = 0; i < n_vals; i++)
        sum += (vals[i]*vals[i]);
    return sum;
}

void reset_vectors(double* A, double* L, double* U, int n_vals)
{
    for (int i = 0; i < n_vals; i++)
    {
        A[i] = i;
        U[i] = 0;
        L[i] = 0;
    }
}


// This is the main method that will be executed when running the program
int main(int argc, char* argv[])
{
    // Declare three large arrays
    int n_vals = 50000;
    double* A_vals = (double*)malloc(n_vals*sizeof(double));
    double* L_vals = (double*)malloc(n_vals*sizeof(double));
    double* U_vals = (double*)malloc(n_vals*sizeof(double));
    double sum_L, sum_U;
    double start, end;


    // Initialize the arrays
    reset_vectors(A_vals, L_vals, U_vals, n_vals);

    // Time a loop that copies values A to appropriate vector
    // Using 'if' statments
    printf("Copying values with 'if' statements\n");
    start = get_time();
    for (int i = 0; i < n_vals; i++)
    {
        for (int j = 0; j < n_vals; j++)
        {
            if (i < j)
            {
                L_vals[i] = A_vals[j];
            }
            else
            {
                U_vals[i] = A_vals[j];
            }
        }
    }
    end = get_time();
    sum_L = norm(L_vals, n_vals);
    sum_U = norm(U_vals, n_vals);
    printf("Norm L %e, Norm U %e\n", sum_L, sum_U); // error checking
    printf("Elapsed %e\n\n", end - start);



    // Initialize the arrays
    reset_vectors(A_vals, L_vals, U_vals, n_vals);

    // Time a loop that copies values A to appropriate vector
    // NOT using 'if' statments
    printf("Copying values without 'if' statements\n");    
    start = get_time();
    for (int i = 0; i < n_vals; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            U_vals[i] = A_vals[j];
        }
        for (int j = i+1; j < n_vals; j++)
        {
            L_vals[i] = A_vals[j];
        }
    }
    end = get_time();
    sum_L = norm(L_vals, n_vals);
    sum_U = norm(U_vals, n_vals);
    printf("Norm L %e, Norm U %e\n", sum_L, sum_U);  // error checking
    printf("Elapsed restructured %e\n", end - start);
    

    return 0;   
}
