#include <stdio.h>
#include <mpi.h>


int main(int argc, char** argv)
{


    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    printf("%s | Rank %i | World Size %i\n", "Hello World!", world_rank, world_size);


    MPI_Finalize();

    return 0;
}

// mpicc hello_world.c -o hello_world
// mpirun --oversubscribe -np 2 ./hello_world
// laptop can only do 2 processes max :(