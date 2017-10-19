//run
//$HOME/opt/usr/local/bin/mpicc -o main ./Desktop/mpi/main.c
//$HOME/opt/usr/local/bin/mpirun -np 4 ./main

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// N>=16 is not working
#define N   4      //matrix size

MPI_Status status;
void printMatrix (int matrix[N][N]);
int main(int argc, char **argv)
{
    int nproc,taskId,source,dest,rows,offset,i,j,k;
    MPI_Datatype type;
    int result[2][2] = {0};
    int resultFinal[N][N] = {0};
    int a[N][N],b[N][N];
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskId);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    
    MPI_Type_vector(4, 2, 4, MPI_INT, &type);
    MPI_Type_commit(&type);
    
//    printf("******************************************************************:\n");
    //root
    if (taskId == 0) {
//        srand( time(NULL) );
        //Generate two NxN matrix
        for (i=0; i<N; i++) {
            for (j=0; j<N; j++) {
                a[i][j]= rand() % 3;
                b[i][j]= rand() % 3;
            }
        }
        
        printf("First matrix:\n");
        printMatrix(a);

        printf("Second matrix:\n");
        printMatrix(b);
 
//      First matrix first block
        MPI_Send(&a[0][0], 2*N, MPI_INT,0,0, MPI_COMM_WORLD);
        MPI_Send(&a[0][0], 2*N, MPI_INT,1,1, MPI_COMM_WORLD);

//      First matrix second block
        MPI_Send(&a[2][0], 2*N, MPI_INT,2,2, MPI_COMM_WORLD);
        MPI_Send(&a[2][0], 2*N, MPI_INT,3,3, MPI_COMM_WORLD);
        
//      Second matrix first block
        MPI_Send(&b[0][0], 1, type, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&b[0][0], 1, type, 2, 2, MPI_COMM_WORLD);

//      Second matrix second block
        MPI_Send(&b[0][2], 1, type, 1, 1, MPI_COMM_WORLD);
        MPI_Send(&b[0][2], 1, type, 3, 3, MPI_COMM_WORLD);
        
    }
    
    //workers
    source = 0;

    MPI_Recv(&a, 2*N, MPI_INT, source, taskId, MPI_COMM_WORLD, &status);
    MPI_Recv(&b, 1, type, source, taskId, MPI_COMM_WORLD, &status);
    
//    printf ("Fist received array \n");
//    for (i=0; i<2; i++) {
//        for (j=0; j<4; j++)
//            printf("%d \t", a[i][j]);
//        printf (" Task id %d \n",taskId);
//    }
//
//    printf ("Second received array \n");
//    for (i=0; i<4; i++) {
//        for (j=0; j<2; j++)
//            printf("%d \t", b[i][j]);
//         printf (" Task id %d \n",taskId);
//    }

    //multiplication
    for (k=0; k<2; k++)
        for (i=0; i<2; i++) {
            for (j=0; j<4; j++)
                result[i][k] = result[i][k] + a[i][j] * b[j][k];
        }
    
//    for (k=0; k<2; k++) {
//        for (j=0; j<2; j++)
//            printf("%d \t", result[k][j]);
//        printf ("Task = %d multiplication result \n",taskId);
//    }

//    printf ("Send by Task id %d \n",taskId);
    MPI_Send(&result[0][0], 2*2, MPI_INT, 0, 4, MPI_COMM_WORLD);
//
//  root receives results
    if(taskId == 0)
    {
        for (i=0; i<nproc; i++)
        {
            source = i;
            if(i == 0)
            {
                MPI_Recv(&result, 2*2, MPI_INT, source, 4, MPI_COMM_WORLD, &status);
                
                for(k = 0; k<2; k++)
                    for (j= 0; j<2; j++)
                        resultFinal[k][j] = result[k][j];
                
            }
            else if(i == 1)
            {
                MPI_Recv(&result, 2*2, MPI_INT, source, 4, MPI_COMM_WORLD, &status);
                for(k = 0; k<2; k++)
                    for (j= 0; j<2; j++)
                        resultFinal[k][j+2] = result[k][j];
            }
            else if(i == 2)
            {
                MPI_Recv(&result, 2*2, MPI_INT, source, 4, MPI_COMM_WORLD, &status);
                for(k = 0; k<2; k++)
                    for (j= 0; j<2; j++)
                        resultFinal[k+2][j] = result[k][j];
            }
            else if(i == 3)
            {
                MPI_Recv(&result, 2*2, MPI_INT, source, 4, MPI_COMM_WORLD, &status);
                for(k = 0; k<2; k++)
                    for (j= 0; j<2; j++)
                        resultFinal[k+2][j+2] = result[k][j];
            }
//            else if(i == 3)
//                MPI_Recv(&resultFinal[2][2], 2*2, MPI_INT, source, 4, MPI_COMM_WORLD, &status);
            
            
//            if(i == 0)
//            {
//                MPI_Recv(&result, 2*2, MPI_INT, source, 4, MPI_COMM_WORLD, &status);
//                for (k=0; k<2; k++) {
//                    for (j=0; j<2; j++)
//                        printf("%d \t", result[k][j]);
//                    printf ("Result nproc id %d \n",i);
//                }
//            }
//            else if(i == 1)
//            {
//                MPI_Recv(&result, 2*2, MPI_INT, source, 4, MPI_COMM_WORLD, &status);
//                for (k=0; k<2; k++) {
//                    for (j=0; j<2; j++)
//                        printf("%d \t", result[k][j]);
//                    printf ("Result nproc id %d \n",i);
//                }
//            }
//            else if(i == 2)
//            {
//                MPI_Recv(&result, 2*2, MPI_INT, source, 4, MPI_COMM_WORLD, &status);
//                for (k=0; k<2; k++) {
//                    for (j=0; j<2; j++)
//                        printf("%d \t", result[k][j]);
//                    printf ("Result nproc id %d \n",i);
//                }
//            }
//            else if(i == 3)
//            {
//                MPI_Recv(&result, 2*2, MPI_INT, source, 4, MPI_COMM_WORLD, &status);
//                for (k=0; k<2; k++) {
//                    for (j=0; j<2; j++)
//                        printf("%d \t", result[k][j]);
//                    printf ("Result nproc id %d \n",i);
//                }
//            }
//
            

        }

        for (k=0; k<4; k++) {
            for (j=0; j<4; j++)
                printf("%d \t", resultFinal[k][j]);
            printf ("\n");
        }
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


