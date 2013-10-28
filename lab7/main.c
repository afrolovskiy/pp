#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ROOT 0
#define SIZE 15


int check(int ridx, int cidx, int *queens){
  int i, delta;
  for (i = 0; i < ridx; i++){
    if (queens[i] == cidx)
      return 0;
    delta = ridx - i;
    if (queens[i] - delta == cidx || queens[i] + delta == cidx)
      return 0;
  }
  return 1;
}


int set_queen(int ridx, int cidx, int *queens){
  int j, count = 0;

  queens[ridx] = cidx;

  if (ridx == SIZE - 1){
    return 1;
  }

  for (j = 0; j < SIZE; j++){
    if (check(ridx+1, j, queens))
      count += set_queen(ridx+1, j, queens);
  }
  return count;
}


int start_idx(int rank, int load, int rest){
  if (rank < rest)
    return rank*(load + 1);
  return rank*load + rest;
}


int end_idx(int rank, int load, int rest){
  if (rank < rest)
    return rank*(load+1) + load + 1;
  return rank*load + rest + load;
}

int main(int argc,char *argv[]){
  int size, rank;
  int j, count = 0, gcount;
  int queens[SIZE];
  int load, rest; 
  int sidx, eidx;
  double t1, t2;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == ROOT)
    t1 = MPI_Wtime();

  load = (SIZE/2 + SIZE%2)/size;
  rest = (SIZE/2 + SIZE%2)%size;
  sidx = start_idx(rank, load, rest);
  eidx = end_idx(rank, load, rest);
 
  for (j = sidx; j < eidx; j++){
    if (SIZE%2 && j == SIZE/2)
      count += set_queen(0, j, queens);
    else
      count += 2*set_queen(0, j, queens);
  }

  MPI_Reduce(&count, &gcount, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD);
  if (rank == ROOT){
    printf("%d\n", gcount);
    t2 = MPI_Wtime();
    printf("elapsed time: %1.2f\n", t2-t1);
  }

  MPI_Finalize();
  return 0;
}
