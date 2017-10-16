//run
//$HOME/opt/usr/local/bin/mpicc -o main ./Desktop/mpi/main.c
//$HOME/opt/usr/local/bin/mpirun -np 4 ./main

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// N>=16 is not working
#define N   16      //matrix size

MPI_Status status;
void printMatrix (int matrix[N][N]);
int main(int argc, char **argv)
{
    int nproc,taskId,source,dest,rows,offset,i,j,k;
    MPI_Datatype type;
    int a[N][N],b[N][N],result[N][N];
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskId);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    
    MPI_Type_vector(16, 8, 16, MPI_INT, &type);
    MPI_Type_commit(&type);
    
    //root
    if (taskId == 0) {
        srand( time(NULL) );
        //Generate two NxN matrix
        for (i=0; i<N; i++) {
            for (j=0; j<N; j++) {
                a[i][j]= 1;//rand() % 10;
                b[i][j]= 2;//rand() % 10;
            }
        }
        
  
        printf("First matrix:\n");
        printMatrix(a);
        
        printf("Second matrix:\n");
        printMatrix(b);
 
//        //send data to worker = 0
//        //First matrix first block
        MPI_Send(&a[0][0], 8*N, MPI_INT,0,1, MPI_COMM_WORLD);
//        MPI_Send(&a[0][0], 8*N, MPI_INT,1,1, MPI_COMM_WORLD);
//
//        //First matrix second block
//        MPI_Send(&a[8][0], 8*N, MPI_INT,2,1, MPI_COMM_WORLD);
//        MPI_Send(&a[8][0], 8*N, MPI_INT,3,1, MPI_COMM_WORLD);
        
        //Second matrix first block
        MPI_Send(&b[0][0], 1, type, 0, 1, MPI_COMM_WORLD);
//        MPI_Send(&b[0][0], 1, type, 2, 1, MPI_COMM_WORLD);
//
//        //Second matrix second block
//        MPI_Send(&b[0][8], 1, type, 1, 1, MPI_COMM_WORLD);
//        MPI_Send(&b[0][8], 1, type, 3, 1, MPI_COMM_WORLD);
        
    }
    
    //worker
    source = 0;

    
    
    MPI_Recv(&a, 8*N, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(&b, 1, type, source, 1, MPI_COMM_WORLD, &status);
    
    for (i=0; i<8; i++) {
        for (j=0; j<16; j++)
            printf("%d \t", a[i][j]);
        printf ("\n");
    }
    
        for (i=0; i<16; i++) {
            for (j=0; j<8; j++)
                printf("%d \t", b[i][j]);
            printf ("\n");
        }
    
    //multiplication
    for (k=0; k<8; k++)
        for (i=0; i<8; i++) {
            for (j=0; j<16; j++)
                result[i][k] = result[i][k] + a[i][j] * b[j][k];
        }
    
    printf ("Result\n");
    for (i=0; i<16; i++) {
        for (j=0; j<16; j++)
            printf("%d \t", result[i][j]);
        printf ("\n");
    }


//    MPI_Send(&result, 8*8, MPI_INT, 0, 2, MPI_COMM_WORLD);
//
//    //root receives results
//    if(taskId == 0)
//    {
////        for (i=0; i<nproc; i++)
////        {
////            source = i;
////            MPI_Recv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
////            MPI_Recv(&rows, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
////            MPI_Recv(&c[offset][0], rows*N, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
////        }
//
//        printf("Result matrix:\n");
//        //printMatrix(b);
//
//        for (i=0; i<4; i++) {
//            for (j=0; j<1; j++)
//                printf("%d \t", b[i][j]);
//            printf ("\n");
//        }
//    }
    
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


