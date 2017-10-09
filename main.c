#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// N>=16 is not working
#define N   15      //matrix size

MPI_Status status;
void printMatrix (int matrix[N][N]);
int main(int argc, char **argv)
{
    int nproc,taskId,source,dest,rows,offset,i,j,k,rowPlus;
    int a[N][N],b[N][N],c[N][N];
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskId);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    
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
        
        rows = N/nproc;
        rowPlus = N%nproc;
        offset = 0;
        
        //Send data to workers
        for (dest=0; dest<nproc; dest++)
        {
            if(rowPlus>0)
            {
                if(dest+1 == nproc)
                    rows = rows + rowPlus;
            }
            MPI_Send(&offset, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            MPI_Send(&rows, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            MPI_Send(&a[offset][0], rows*N, MPI_INT,dest,1, MPI_COMM_WORLD);
            MPI_Send(&b, N*N, MPI_INT, dest, 1, MPI_COMM_WORLD);
            
            //printf("dest: %d \n", dest);
            //printf("rows: %d \n", rows);
            //printf("offset: %d \n", offset);
            
            offset = offset + rows;
        }
    }
    
    //worker
    source = 0;
    MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(&a, rows*N, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(&b, N*N, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
    
    //multiplication
    for (k=0; k<N; k++)
        for (i=0; i<rows; i++) {
            for (j=0; j<N; j++)
                c[i][k] = c[i][k] + a[i][j] * b[j][k];
        }
    
    MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    MPI_Send(&c, rows*N, MPI_INT, 0, 2, MPI_COMM_WORLD);
    
    //root receives results
    if(taskId == 0)
    {
        for (i=0; i<nproc; i++)
        {
            source = i;
            MPI_Recv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
            MPI_Recv(&rows, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
            MPI_Recv(&c[offset][0], rows*N, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
        }
        
        printf("Result matrix:\n");
        printMatrix(c);
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

