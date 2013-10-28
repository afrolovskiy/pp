#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define COLUMNS 10
#define ROWS 2


int **alloc_matrix(int nrows, int ncols){
  int i;
  int *data = (int *) malloc(nrows*ncols*sizeof(int));
  int **arr = (int **) malloc(nrows*sizeof(int *));
  for (i=0; i< nrows; i++)
    arr[i] = &(data[i*ncols]);
  /*
  int i;
  int **arr = (int **) malloc(nrows*sizeof(int *));
  for (i=0; i< nrows; i++)
    arr[i] = (int *) malloc(ncols*sizeof(int));
  */
  return arr;
}

void fill_matrix(int **arr, int nrows, int ncols){
  int i, j, tmp = 0;
  for (i=0; i<nrows; i++){
    for (j=0; j<ncols; j++)
      arr[i][j] = tmp++;
  }
}

void print_matrix(int **arr, int nrows, int ncols){
  int i, j;
  for (i=0; i<nrows; i++){
    for (j=0; j<ncols; j++)
      printf("%5d ", arr[i][j]);
    printf("\n");
  }
}

int main(int argc, char **argv){
  
  int rank;
  MPI_Status status;
  MPI_Datatype column, transposed;
  MPI_Request request;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); 

  int **send_matr = alloc_matrix(ROWS, COLUMNS);
  fill_matrix(send_matr, ROWS, COLUMNS);
  int **recv_matr = alloc_matrix(COLUMNS, ROWS);

  MPI_Type_vector(ROWS, 1, COLUMNS, MPI_INT, &column);
  MPI_Type_commit(&column);
  MPI_Type_hvector(COLUMNS, 1, sizeof(int), column, &transposed);
  MPI_Type_commit(&transposed);
  
  printf("Send matrix:\n");
  print_matrix(send_matr, ROWS, COLUMNS);
  
  MPI_Isend(&(send_matr[0][0]), 1, transposed, 0, 1, MPI_COMM_WORLD, &request);
  MPI_Recv(&(recv_matr[0][0]), ROWS*COLUMNS, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
  
  printf("Received matrix:\n");
  print_matrix(recv_matr, COLUMNS, ROWS);
  
  MPI_Type_free(&column);
  MPI_Type_free(&transposed);
  
  MPI_Finalize();
  return 0;
}
