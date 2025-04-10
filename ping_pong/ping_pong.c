#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>

// for getting average time per message
float getAvg(double arr[], int n) {
    double sum = 0.0;

    // Find the sum of all elements
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
      
      // Return the average
    return sum / (double)n;
}

int main(int argc, char *argv[])
{
    
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (argc != 2) {
        MPI_Finalize();
        printf("Incorrect number of arguments: ./ping_pong MessageLengthSize\n");
        return -1;
    }

    // number of iterations and message size
    int N_iter = 100000;
    int M_size = atoi(argv[1]);

    int count = 0;
    int next_rank;
    int prev_rank;


    // for Buffer Send mode
    int *msg;
    int packet_size;
    int buf_size;

    MPI_Pack_size( M_size, MPI_INT, MPI_COMM_WORLD, &packet_size );
    buf_size = (packet_size + MPI_BSEND_OVERHEAD) * N_iter * 2; 
    MPI_Buffer_attach( &msg, buf_size );


    // for Send and Ssend mode
    // int *msg = malloc( M_size * sizeof(int) );
    double  *tpm = malloc( N_iter * sizeof(double) );        // time per message (used to calculate an average for message time)

    // only rank 0 has message data of size M_size at start
    if ( world_rank == 0 )
    {
        for (int i = 0; i < M_size; i++)
        {
            msg[i] = i;
        }
    }


    double Tstart, Tend;        // time per message
    double TGstart, TGend;      // global total time

    MPI_Barrier(MPI_COMM_WORLD);
    TGstart = MPI_Wtime(); 

    // sends ping pong messages onlt between proc 0 and 1
    if (world_rank <= 1)
    {
        while (count < N_iter)
        {
            count++;
            next_rank = ( world_rank + 1 ) % 2;
            prev_rank = ( world_rank - 1 ) % 2;

            MPI_Barrier(MPI_COMM_WORLD);
            Tstart = MPI_Wtime();  

            if ( count % 2 != world_rank )
            {
                // MPI_Buffer_attach( msg, buf_size );
                MPI_Bsend(msg, M_size, MPI_INT, next_rank, 0, MPI_COMM_WORLD);
                // printf("Proc %d sent a message\n", world_rank);   
            }
            else
            {
                MPI_Recv(msg, M_size, MPI_INT,  next_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                // printf("Proc %d recv a message\n", world_rank);
                // MPI_Buffer_detach( msg, &buf_size );
            }

            MPI_Barrier(MPI_COMM_WORLD);
            Tend = MPI_Wtime();
            if (world_rank == 0) 
            {
                // printf("Iteration %d | Execution Time: %fs\n", count, Tend - Tstart);
                tpm[count-1] = Tend - Tstart;
            }

        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    TGend = MPI_Wtime();
    if (world_rank == 0) 
    {
        printf("Total Execution Time: %fs\n", TGend - TGstart);

        double avg_tpm = getAvg(tpm, N_iter);
        printf("Average time per message: %.10fs\n", avg_tpm);
    }


    MPI_Finalize();

    return 0;
}


// mpicc ping_pong.c -o ping_pong
// mpirun -np 2 ./ping_pong