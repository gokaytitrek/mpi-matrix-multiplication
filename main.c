//run
//$HOME/opt/usr/local/bin/mpicc -o main ./Desktop/mpi/main.c
//$HOME/opt/usr/local/bin/mpirun -np 4 ./main

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// N>=16 is not working
#define N   16      //matrix size
#define BS  N/2     //block size

MPI_Status status;
void printMatrix (int matrix[N][N]);
int main(int argc, char **argv)
{
    int nproc,taskId,source,i,j,k,positionX,positionY;
    
    MPI_Datatype type;
    int result[BS][BS] = {0};
    int resultFinal[N][N] = {0};
    int a[N][N],b[N][N];
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskId);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    
    MPI_Type_vector(N, BS, N, MPI_INT, &type);
    MPI_Type_commit(&type);
    
    //root
    if (taskId == 0) {
        srand( time(NULL) );
        //Generate two NxN matrix
        for (i=0; i<N; i++) {
            for (j=0; j<N; j++) {
                a[i][j]= rand() % 10;
                b[i][j]= rand() % 10;
            }
        }
        
        printf("First matrix:\n");
        printMatrix(a);

        printf("Second matrix:\n");
        printMatrix(b);
 
//      First matrix first block
        MPI_Send(&a[0][0], BS*N, MPI_INT,0,0, MPI_COMM_WORLD);
        MPI_Send(&a[0][0], BS*N, MPI_INT,1,1, MPI_COMM_WORLD);

//      First matrix second block
        MPI_Send(&a[BS][0], BS*N, MPI_INT,2,2, MPI_COMM_WORLD);
        MPI_Send(&a[BS][0], BS*N, MPI_INT,3,3, MPI_COMM_WORLD);
        
//      Second matrix first block
        MPI_Send(&b[0][0], 1, type, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&b[0][0], 1, type, 2, 2, MPI_COMM_WORLD);

//      Second matrix second block
        MPI_Send(&b[0][BS], 1, type, 1, 1, MPI_COMM_WORLD);
        MPI_Send(&b[0][BS], 1, type, 3, 3, MPI_COMM_WORLD);
        
    }
    
    //workers
    source = 0;

    MPI_Recv(&a, BS*N, MPI_INT, source, taskId, MPI_COMM_WORLD, &status);
    MPI_Recv(&b, 1, type, source, taskId, MPI_COMM_WORLD, &status);
    
    MPI_Type_free(&type);
    
    //multiplication
    for (k=0; k<BS; k++)
        for (i=0; i<BS; i++) {
            for (j=0; j<N; j++)
                result[i][k] = result[i][k] + a[i][j] * b[j][k];
        }
    
    //Send result to root
    MPI_Send(&result[0][0], BS*BS, MPI_INT, 0, 4, MPI_COMM_WORLD);

    //root receives results
    if(taskId == 0)
    {
        for (i=0; i<nproc; i++)
        {
            source = i;
            MPI_Recv(&result, BS*BS, MPI_INT, source, 4, MPI_COMM_WORLD, &status);
            //Manage shifting
            if(source == 0)
            {
                positionX = 0;
                positionY = 0;
            }
            else if(source == 1)
            {
                positionX = 0;
                positionY = BS;
            }
            else if(source == 2)
            {
                positionX = BS;
                positionY = 0;
            }
            else if(source == 3)
            {
                positionX = BS;
                positionY = BS;
            }
            
            for(k = 0; k<BS; k++)
                for (j= 0; j<BS; j++)
                    resultFinal[k+positionX][j+positionY] = result[k][j];
        }

        printf("Result matrix:\n");
        printMatrix(resultFinal);
    }

    MPI_Finalize();
}

void printMatrix (int matrix[N][N])
{
    int i,j;
    for (i=0; i<N; i++) {
        for (j=0; j<N; j++)
            printf("%d \t", matrix[i][j]);
        printf ("\n");
    }
    printf ("\n");
}


