#include <stdlib.h>
#include <stdio.h>
#include "../timer.h"
#include <omp.h>
#include <vector>

int main(int argc, char* argv[])
{
    int thread_id, num_threads;
#pragma omp parallel
{
#pragma omp for nowait
    for (int i = 0; i < 6; i++)
    {
        omp_set_num_threads(i);
    thread_id = omp_get_thread_num();
    num_threads = omp_get_num_threads();
    printf("Hello world from thread %d of %d\n", thread_id, num_threads);
    }
}

    return 0;
}
