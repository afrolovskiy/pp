#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ROOT 0
#define COUNT 10


int main(int argc,char *argv[]){
  int size, rank;
  int i, tcount, sum, rsum, gsum;
  int *larr, *sarr;
  double t1, t2;
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  larr = (int *) malloc(COUNT*sizeof(int));
  
  if (rank == ROOT){
    /* root sent data to all processors */
    tcount = COUNT * size;
    sarr = (int *) malloc(tcount*sizeof(int));
    for (i=0; i<tcount; i++){
      sarr[i] = i;
    }
  }
  MPI_Scatter(sarr, COUNT, MPI_INT, larr, COUNT, MPI_INT, ROOT, MPI_COMM_WORLD);

  /* each processor calculate sum of array elements*/
  sum = 0;
  for (i=0; i<COUNT; i++){
    sum += larr[i];
  }
  /*printf("rank = %d, sum = %d\n", rank, sum);*/

  /* each processor sent result to root and root summarize its */
  /* with MPI_Reduce */
  t1 = MPI_Wtime();
  MPI_Reduce(&sum, &gsum, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD);
  t2 = MPI_Wtime();

  if (rank == ROOT){
    printf("global sum = %d\n", gsum);
    printf("MPI_Reduce elapsed time: %1.2f\n", t2-t1);
  }

  /* without MPI_Reduce */
  t1 = MPI_Wtime();
  if (rank == ROOT){
    gsum = 0;
    for (i=0; i<size; i++){
      if (i != ROOT){
        MPI_Recv(&rsum, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
        gsum += rsum;
      } else {
        gsum += sum;
      }
    }
  } else {
    MPI_Send(&sum, 1, MPI_INT, ROOT, 0, MPI_COMM_WORLD);
  }
  t2 = MPI_Wtime();

  if (rank == ROOT){
    printf("global sum = %d\n", gsum);
    printf("Without MPI_Reduce elapsed time: %1.2f\n", t2-t1);
  }

  if (rank == ROOT){
    free(sarr);
  }

  free(larr);
  MPI_Finalize();
  return 0;
}
