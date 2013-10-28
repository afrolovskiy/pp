#include <stdio.h>
#include <mpi.h>


int main( int argc, char **argv ){
  
  int size, rank, lnm;
  /* 
   size - количество узлов
   rank - номер узла
   lnm - длина имени процессора
  */
  char nm[MPI_MAX_PROCESSOR_NAME+1]; /* имя процессора */
   
  MPI_Init( &argc, &argv );
  
  MPI_Get_processor_name( nm, &lnm );
  
  MPI_Comm_size( MPI_COMM_WORLD, &size ); /* MPI_COMM_WORLD - дескриптор коммуникатора */
  MPI_Comm_rank( MPI_COMM_WORLD, &rank ); 
  
  printf("Hello World!!\nI am %d of %d.\nI am on %s.\n", rank, size, nm );
    
  MPI_Finalize();
  return 0;
}
