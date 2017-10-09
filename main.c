#include <mpi.h>
#include <stdio.h>
//run
//$HOME/opt/usr/local/bin/mpicc -o hello ./Desktop/PhD/mpi/main.c
//$HOME/opt/usr/local/bin/mpirun -np 4 ./hello

int main(int argc, char** argv) {
    MPI_Init(NULL, NULL);
    int rank;
    int world;
    int root,i,err,n, temp;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world);
    printf("Hello: rank %d, world: %d\n",rank, world);
    
    root = 0;
    temp = 89;
    if (rank == root) {
        for (i=1; i<world; i++){
            MPI_Send( &temp, 1, MPI_INTEGER, i, 0, MPI_COMM_WORLD);
        }
    }
    else {
        MPI_Recv( &n, 1, MPI_INTEGER, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
        printf("Process %d received number %d from process 0\n",rank, n);
    }
    
    MPI_Finalize();
}
