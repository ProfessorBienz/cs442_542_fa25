#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>


int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    MPI_Comm new_comm;

    MPI_Comm_split(MPI_COMM_WORLD, rank % 2, rank, &new_comm);

    int new_rank, new_size;
    MPI_Comm_rank(new_comm, &new_rank);
    MPI_Comm_size(new_comm, &new_size);
    if (rank % 2 == 0)
    { 
        MPI_Comm new_new_comm;
        MPI_Comm_split(MPI_COMM_WORLD, rank, rank, &new_new_comm);
        printf("Rank %d of %d, even rank %d of %d\n",rank, num_procs, new_rank, new_size);
     }
    else
       printf("Rank %d of %d, odd rank %d of %d\n", rank, num_procs, new_rank, new_size); 

    MPI_Comm_free(&new_comm);

    return MPI_Finalize();
}
