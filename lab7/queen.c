#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 16

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


long int set_queen(int ridx, int cidx, int *queens){
  int j;
  long int  count = 0;
  
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


int main(int argc,char *argv[]){
  int j;
  long int count = 0;
  int queens[SIZE];
  clock_t begin, end;

  begin = clock();
  for (j = 0; j < SIZE; j++)
    count += set_queen(0, j, queens);
  end = clock();

  printf("count = %d, matrix size: %dx%d\n", count, SIZE, SIZE);
  printf("elapsed time: %1.2f\n", ((double)end - begin) / CLOCKS_PER_SEC);
  return 0;
}
