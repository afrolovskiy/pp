#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define SENDER 0
#define RECEIVER 1

#define ITER_COUNT 50
#define INIT_BUF_SIZE 20971520 /* 1024*1024*10 = 10 Mb */
#define BUF_STEP 5242880 /* 1024*1024*5 = 5 Mb */
#define MAX_BUF_SIZE 104857600 /* 1024*1024*100 = 100 Mb */


int main( int argc, char **argv ){
  
  int rank, bsize, i;
  char *buf;
  /*time_t stime, etime;*/
  double stime, etime;
  double exec_time, mbsize, pfmc;
  MPI_Status status;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); 

  for (bsize = INIT_BUF_SIZE; bsize <= MAX_BUF_SIZE; bsize += BUF_STEP){
    buf = malloc(sizeof(char) * bsize);

    if (rank == SENDER){
      /*time(&stime);*/
      stime = MPI_Wtime();
      for (i = 0; i < ITER_COUNT; i++){
        MPI_Ssend(buf, bsize, MPI_CHAR, RECEIVER, 0, MPI_COMM_WORLD);
      }
      /*time(&etime);*/
      etime = MPI_Wtime();

      /*exec_time = difftime(etime, stime) / (double) ITER_COUNT;*/
      exec_time = (etime - stime) / (double) ITER_COUNT;
      mbsize = (double) bsize / 1024 / 1024;
      pfmc = mbsize / exec_time;
      printf("Buffer size: %4.1fMb\tTime elapsed: %7.6f sec\tPerformence: %f Mb/sec\n", mbsize, exec_time, pfmc);
    } else if (rank == RECEIVER){
      for (i = 0; i < ITER_COUNT; i++){
        MPI_Recv(buf, bsize, MPI_CHAR, SENDER, 0, MPI_COMM_WORLD, &status);
      }
    }

    free(buf);
  }
    
  MPI_Finalize();
  return 0;
}
