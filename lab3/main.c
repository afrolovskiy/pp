#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int neighbour(int rank, int size){
  return (rank + 1) % size;
}

int main(int argc, char **argv){
  
  int rank, size, *buf, **bufs, i, sum = 0, count = 0;
  MPI_Status status;
  MPI_Request request;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  bufs = malloc(sizeof(int*) * size);
  buf = malloc(sizeof(int));
  buf[0] = rank;
  bufs[0] = buf;

  MPI_Issend(buf, 1, MPI_INT, neighbour(rank, size), 0, MPI_COMM_WORLD, &request);
  printf("Processor %d sent message with value %d\n", rank, buf[0]);

  while (count < size){
    count++;    
    
    buf = malloc(sizeof(int));
    bufs[count] = buf;

    MPI_Recv(buf, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
    if (buf[0] == rank) {
      printf("Processor %d received self message\n", rank);
    } else {
      MPI_Issend(buf, 1, MPI_INT, neighbour(rank, size), 0, MPI_COMM_WORLD, &request);
      printf("Processor %d received and sent message with value %d\n", rank, buf[0]);
    }

    sum += buf[0];
  }

  for (i = 0; i < size; i++){
    free(bufs[i]);
  }
  free(bufs);
  
  printf("Processor %d stoped, sum = %d\n", rank, sum);
  MPI_Finalize();
  return 0;
}
