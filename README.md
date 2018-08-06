# Matrix Multiplication using MPI

![alt text](http://i65.tinypic.com/9pxavs.jpg)


Create 4 worker processes. Process 0 initializes matrices A and B
randomly, partitions the data, and distributes the partitions to the other workers. Each worker calculates its own partition
of the result matrix C. After the calculations, Process 0 receives the results from the other processes and displays matrix
C on the screen. Partition the matrices as given in the following figure.

# Compile and Run
```console
$HOME/opt/usr/local/bin/mpicc -o main ./Desktop/mpi/main.c
$HOME/opt/usr/local/bin/mpirun -np 4 ./main
```


This project was developed to be a parallel programming example with MPI.

