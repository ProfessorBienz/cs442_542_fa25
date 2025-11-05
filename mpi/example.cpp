#include "mpi.h"
#include "stdlib.h"
#include "stdio.h"

void send_column(int* mat, int* col, int n, int idx)
{
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status status;

}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    int num_procs, rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if (argc <= 1)
    {
        if (rank == 0) printf("Pass a command line argument for matrix dimension\n");
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
    int n = atoi(argv[1]);

    int* mat = new int[n*n];
    if (rank == 0)
        for (int i = 0; i < n*n; i++)
            mat[i] = i;
    int* col = new int[n];


    double t0, tfinal;
    int n_iter = 100;
    int col_idx = 1;


    MPI_Barrier(MPI_COMM_WORLD);
    t0 = MPI_Wtime();
    for (int i = 0; i < n_iter; i++)
        send_column(mat, col, n, col_idx);
    tfinal = (MPI_Wtime() - t0) / n_iter;
    MPI_Reduce(&tfinal, &t0, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (rank == 0) printf("Cost of Sending Column: %e\n", t0);



    delete[] col;
    delete[] mat;

    MPI_Finalize();
    return 0;
}
