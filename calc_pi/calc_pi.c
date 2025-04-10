#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    double Tstart, Tend;    
    MPI_Barrier(MPI_COMM_WORLD);
    Tstart = MPI_Wtime();    

    if (argc != 2) {
        MPI_Finalize();
        printf("Program Aborted | Parameter N required -> ./calc_pi N\n");
        return -1;
    }

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int N = atoi(argv[1]);
    int start, end;

    // load balancing across processes
    int np = world_size;
    int chunk_size = floor(N/np);
    int rmd = N % chunk_size;

    start = (world_rank * chunk_size) + 1;
    end = (world_rank + 1) * chunk_size; 

    // Adding remainder elements to last rank
    while (rmd != 0) 
    {
        if (world_rank == world_size - 1) {
            end++;
        }
        rmd--;        
    }

    printf("INDEX | start %d | end %d\n", start, end);
    // MPI_Barrier(MPI_COMM_WORLD);

    // float forth_pi;     // testing single precision
    double forth_pi;    // testing double precision
    
    forth_pi = 0.0;

    for (int i=start; i <= end; i++)
    {
        // float denominator;      // testing single precision
        double denominator;     // testing double precision

        denominator = 1 + pow( ( (i-0.5) / N ), 2);
        forth_pi += 1.0 / denominator;
    }


    if (world_rank != 0) 
    {
        MPI_Ssend(&forth_pi, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    } 
    else 
    {
        // float data;     // testing single precision
        double data;    // testing double precision

        for (int i=1; i<world_size; i++) {
            MPI_Recv(&data, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);                        // Recving data in processor order till world_size-1    
            // MPI_Recv(&data, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);           // Recving data using wildcards + adding straight away
            forth_pi += data;                      
        }


        forth_pi *= 1.0 / N;        

        // float pi;        // testing single precision
        double pi;       // testing double precision

        pi = forth_pi * 4.0;

        printf("Rank %d | PI = %.20f\n", world_rank, pi);

    }

    
    MPI_Barrier(MPI_COMM_WORLD);
    Tend = MPI_Wtime();
    if (world_rank == 0) printf("Rank %d | Execution Time: %fs\n", world_rank, Tend - Tstart);

    MPI_Finalize();

    return 0;
}

// mpicc calc_pi.c -o calc_pi -lm
// mpirun --oversubscribe -np 2 ./calc_pi N

// Question 2.2.1
// PI point precision using wilcards
// 1st Run : 3.1415926536
// 2nd Run  : 3.1415953636

// using wildcards vs processes directly
// if you use wildcards the value of the final pi is always slightly different everytime you run it.
// if you use processes directly to recv the messages the final value of pi is always the same.
// ?? why tho do wildcards do this





// Question 2.2.2
// float pi precision (10 decimals)  : 3.1415953636
// double pi precision (20 decimals) : 3.14159265362314332748

// These values are the same everytime I run the calculation