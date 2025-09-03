// This program accesses random positions from some level of memory
// If the array being accessed is small enough, all values with be accessed from L1 cache
// If slightly larger, the values will be accesses from L2 cache
// Very large arrays will be accessed from main memory

#include <stdlib.h>
#include <stdio.h>

// Import timer.h (the other file I have uploaded) 
// as it has all of the timing methods
#include "timer.h"


// This is the main program
int main(int argc, char* argv[])
{
    // This program requires input (the size of the array we are reading)
    if (argc == 1)
    {
        printf("Need input arg\n");
        return 0;
    }

    // This seeds the random number generator, 
    // so it is different every time you run the program
    srand(time(NULL));

    // Initialize the variables
    int n_access = 100000000;
    int size = atoi(argv[1]);
    int n_outer = n_access / size;
    int ptr, tmp;
    double scale = 1.0 * RAND_MAX/size;

    // Create a double array of size 'size' (program input)
    // This is the array we are accessing from memory
    double* vals = (double*)malloc(size*sizeof(double));

    // Random order to step through list
    // Create an 'integer' array which will hold the position in 'vals'
    // that we access at each step.  Sort this array in a random order.
    int* pos = (int*)malloc(size*sizeof(int));
    for (int i = 0; i < size; i++)
        pos[i] = i;
    for (int i = 0; i < size; i++)
    {
        int j = (int) (rand() / scale);
        tmp = pos[i];
        pos[i] = pos[j];
        pos[j] = tmp;
    }
    
    // Warm Up, make sure all of vals is in cache, if it fits
    // Pos was already warmed up (stepped through in previous step)
    int ctr = 0;
    for (int i = 0; i < size; i++)
    {
        vals[i] = 1.0;
    }

    // Start timer
    double start = get_time();

    // Stepping through the arrays pos and vals.  We are stepping through the vals array 
    // in a random order, so if the array is large enough most accesses will be from main memory.
    // Must be random because of 'cache lines' which we will learn about for 08/21/20.
    for (int i = 0; i < n_outer; i++)
    {
        for (int j = 0; j < size; j++)
        {
            ptr = pos[j];
            vals[ptr] *= 2;
        }
    }
    double end = get_time();

    // Get measurements, such as seconds from timeofday, as well as rate at which data is accessed
    double seconds = get_seconds(start, end);

    // For data access rate, including double and int because vals and pos,
    // but this may not be 100% accurate as pos is not out of order and much
    // of this will be in L1 cache so it could be ignored.  May be more accurate
    // to only consider vals for the bytes.
    long bytes = (n_access*sizeof(double) + n_access*sizeof(int));
    double rate = get_rate(seconds, bytes);
    double grate = get_grate(rate);

    // Print out the information about the run to the screen
    printf("Size %d, Seconds %e, Seconds Per Double %e, Gbytes/sec %e\n", size, seconds, seconds / n_access, grate);

    free(vals);
    free(pos);

    return 0;
}










