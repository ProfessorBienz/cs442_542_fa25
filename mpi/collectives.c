#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

// MPI_Reduce(void* sendbuf, void* recvbuf, int count, MPI_Datatype datatype,
//         MPI_Op op, int root, MPI_Comm comm)
//
// MPI_Allreduce(void* sendbuf, void* recvbuf, int count, MPI_Datatype datatype,
//         MPI_Op op, MPI_Comm comm)
//
// MPI_Bcast(void* buf, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
//
// MPI_Scatter(void* sendbuf, int sendcount, MPI_Datatype sendtype,
//         void* recvbuf, int recvcount, MPI_Datatype recvtype, 
//         int root, MPI_Comm comm)
//
// MPI_Gather(void* sendbuf, int sendcount, MPI_Datatype sendtype,
//         void* recvbuf, int recvcount, MPI_Datatype recvtype,
//         int root, MPI_Comm comm)
//         
// MPI_Gatherv(void *sendbuf, int sendcount, MPI_Datatype sendtype,
//         void* recvbuf, int* recvcounts, count int* displs,
//         MPI_Datatype recvtype, int root, MPI_Comm comm)
//
// MPI_Allgather(void* sendbuf, int sendcount, MPI_Datatype sendtype,
//         void* recvbuf, int recvcount, MPI_Datatype recvtype,
//         MPI_Comm comm)
//      
// MPI_Allgatherv(void *sendbuf, int sendcount, MPI_Datatype sendtype,
//         void* recvbuf, int* recvcounts, count int* displs,
//         MPI_Datatype recvtype, MPI_Comm comm)
//
// MPI_Alltoall(void* sendbuf, int sendcout, MPI_Datatype sendtype,
//         void* recvbuf, int recvcount, MPI_Datatype recvtype,
//         MPI_Comm comm)
//
// MPI_Alltoallv(void* sendbuf, int* sendcounts, int* sdispls, 
//         MPI_Datatype sendtype, void* recvbuf, int* recvcounts,
//         int* rdispls, MPI_Datatype recvtype, MPI_Comm comm)
//
// MPI_Barrier(MPI_Comm comm)
//
// MPI_Scan(void* sendbuf, void* recvbuf, int count, MPI_Datatype datatype,
//         MPI_Op op, MPI_Comm comm)
//
// MPI_Exscan(void* sendbuf, void* recvbuf, int count, MPI_Datatype datatype,
//         MPI_Op op, MPI_Comm comm)
//
// MPI_Reduce_scatter(const void *sendbuf, void *recvbuf, const int recvcounts[],
//                     MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)


// Initialize values to round robin processes
// E.g. P0 init_vals={0, num_procs, 2*num_procs, ...}
//      P1 init_vals={1, num_procs+1, 2*num_procs+1, ...}
void initialize(int* vals, int n)
{
    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    for (int i = 0; i < n; i++)
        vals[i] = i*num_procs + rank;

}



// MPI_Reduce(void* sendbuf, void* recvbuf, int count, MPI_Datatype datatype,
//         MPI_Op op, int root, MPI_Comm comm)
// Reducing a list of values to find the maximum, at each position, across all processes
// Returns result only to root (process 0)
void reduce(int n)
{
    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int* init_vals = (int*)malloc(n*sizeof(int));
    int* reduce_vals = (int*)malloc(n*sizeof(int));

    initialize(init_vals, n);

    MPI_Reduce(init_vals, reduce_vals, n, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) 
    {
        for (int i = 0; i < n; i++)
        {
            printf("Max Vals[%d] = %d\n", i, reduce_vals[i]);
        }
    }

    free(init_vals);
    free(reduce_vals);
}

// MPI_Allreduce(void* sendbuf, void* recvbuf, int count, MPI_Datatype datatype,
//         MPI_Op op, MPI_Comm comm)
// Reducing a list of values to find the maximum, at each position, across all processes
// Returns result to all processes
// Process passed as argument prints the values
void allreduce(int n, int print_proc)
{
    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int* init_vals = (int*)malloc(n*sizeof(int));
    int* reduce_vals = (int*)malloc(n*sizeof(int));

    initialize(init_vals, n);

    MPI_Allreduce(init_vals, reduce_vals, n, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

    if (rank == print_proc) 
    {
        for (int i = 0; i < n; i++)
        {
            printf("Max Vals[%d] = %d\n", i, reduce_vals[i]);
        }
    }

    free(init_vals);
    free(reduce_vals);
}

// MPI_Bcast(void* buf, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
void bcast(int n, int print_proc)
{
    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int* vals = (int*)malloc(n*sizeof(int));

    if (rank == 0)
    {
        for (int i = 0; i < n; i++)
        {
            vals[i] = i+1;
        }
    }

    MPI_Bcast(vals, n, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == print_proc)
    {
        for (int i = 0; i < n; i++)
        {
            printf("Bcastd Vals[%d] = %d\n", i, vals[i]);
        }
    }

    free(vals);
}

// MPI_Scatter(void* sendbuf, int sendcount, MPI_Datatype sendtype,
//         void* recvbuf, int recvcount, MPI_Datatype recvtype, 
//         int root, MPI_Comm comm)
void scatter(int n)
{
    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int* init_vals = (int*)malloc(n*num_procs*sizeof(int));
    int* recv_vals = (int*)malloc(n*sizeof(int));
    if (rank == 0) 
    {
        for (int i = 0; i < n*num_procs; i++)
        {
            init_vals[i] = i+1;
        }
    }

    MPI_Scatter(init_vals, n, MPI_INT, recv_vals, n, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < n; i++)
    {
        printf("Rank %d, Recv[%d] = %d\n", rank, i, recv_vals[i]);
    }

    free(init_vals);
    free(recv_vals);
}

// MPI_Gather(void* sendbuf, int sendcount, MPI_Datatype sendtype,
//         void* recvbuf, int recvcount, MPI_Datatype recvtype,
//         int root, MPI_Comm comm)
void gather(int n)
{
    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int* init_vals = (int*)malloc(n*sizeof(int));
    initialize(init_vals, n);

    int* recv_vals;
    if (rank == 0) 
    {
        recv_vals = (int*)malloc(n*num_procs*sizeof(int));
    }

    MPI_Gather(init_vals, n, MPI_INT, recv_vals, n, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) 
    {
        for (int i = 0; i < n*num_procs; i++)
        {
            printf("RecvVals[%d] = %d\n", i, recv_vals[i]);
        }
    }

    free(init_vals);
    if (rank == 0) free(recv_vals);
}

// MPI_Allgather(void* sendbuf, int sendcount, MPI_Datatype sendtype,
//         void* recvbuf, int recvcount, MPI_Datatype recvtype,
//         MPI_Comm comm)
void allgather(int n, int print_proc)
{
    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int* init_vals = (int*)malloc(n*sizeof(int));
    initialize(init_vals, n);
    int* recv_vals = (int*)malloc(n*num_procs*sizeof(int));

    MPI_Allgather(init_vals, n, MPI_INT, recv_vals, n, MPI_INT, MPI_COMM_WORLD);

    if (rank == print_proc) 
    {
        for (int i = 0; i < n*num_procs; i++)
        {
            printf("RecvVals[%d] = %d\n", i, recv_vals[i]);
        }
    }

    free(init_vals);
    free(recv_vals);
}

// MPI_Alltoall(void* sendbuf, int sendcout, MPI_Datatype sendtype,
//         void* recvbuf, int recvcount, MPI_Datatype recvtype,
//         MPI_Comm comm)
void alltoall(int n)
{
    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int* init_vals = (int*)malloc(n*num_procs*sizeof(int));
    int* recv_vals = (int*)malloc(n*num_procs*sizeof(int));
    for (int i = 0; i < n*num_procs; i++)
    {
        init_vals[i] = i + 1;
    }

    MPI_Alltoall(init_vals, n, MPI_INT, recv_vals, n, MPI_INT, MPI_COMM_WORLD);

    for (int i = 0; i < num_procs; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("Rank %d recvd %d from %d\n", rank, recv_vals[i*n+j], i);
        }
    }

    free(init_vals);
    free(recv_vals);
}


// MPI_Scan(void* sendbuf, void* recvbuf, int count, MPI_Datatype datatype,
//         MPI_Op op, MPI_Comm comm)
//
void scan(int n)
{
    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int* init_vals = (int*)malloc(n*sizeof(int));
    int* reduce_vals = (int*)malloc(n*sizeof(int));

    initialize(init_vals, n);

    MPI_Scan(init_vals, reduce_vals, n, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    for (int i = 0; i < n; i++)
    {
        printf("Rank %d, Orig Val[%d] = %d Sum Vals (Scan)[%d] = %d\n", rank, i, init_vals[i], i, reduce_vals[i]);
    }

    free(init_vals);
    free(reduce_vals);
}

// MPI_Exscan(void* sendbuf, void* recvbuf, int count, MPI_Datatype datatype,
//         MPI_Op op, MPI_Comm comm)
//
void exscan(int n)
{
    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int* init_vals = (int*)malloc(n*sizeof(int));
    int* reduce_vals = (int*)malloc(n*sizeof(int));

    initialize(init_vals, n);

    MPI_Exscan(init_vals, reduce_vals, n, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    for (int i = 0; i < n; i++)
    {
        printf("Rank %d, Orig Val[%d] = %d Sum Vals (Exscan)[%d] = %d\n", rank, i, init_vals[i], i, reduce_vals[i]);
    }

    free(init_vals);
    free(reduce_vals);
}


// MPI_Reduce_scatter(const void *sendbuf, void *recvbuf, const int recvcounts[],
//                     MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
void reduce_scatter(int n)
{
    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int* init_vals = (int*)malloc(n*num_procs*sizeof(int));
    int* reduce_vals = (int*)malloc(n*sizeof(int));
    int* recvcounts = (int*)malloc(num_procs*sizeof(int));

    for (int i = 0; i < num_procs; i++)
    {
        for (int j = 0; j < n; j++)
        {
            init_vals[i*n+j] = i;
        }
        recvcounts[i] = n;
    }

    MPI_Reduce_scatter(init_vals, reduce_vals, recvcounts, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    for (int i = 0; i < n; i++)
    {
        printf("Rank %d, Reduce-Scatter[%d] = %d\n", rank, i, reduce_vals[i]);
    }

    free(init_vals);
    free(reduce_vals);
    free(recvcounts);
}


int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int n = 1;
    if (argc > 1) n = atoi(argv[1]);

    //reduce(n);

    //allreduce(n, 1);
    
    //bcast(n, 1);
    
    //scatter(n);

    //gather(n);

    //allgather(n, 1);
    
    //alltoall(n);
    
    //scan(n);
    
    //exscan(n);

    //reduce_scatter(n);


    return 0;
}





