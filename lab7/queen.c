#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 15

void print_board(int *queens){
  int i, j;
  for (i = 0; i < SIZE; i++){
    for (j = 0; j < SIZE; j++){
      if (queens[i] == j)
        printf("1");
      else
        printf("0");
    }
    printf("\n");
  }
}


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


int main(int argc,char *argv[]){
  int j, count = 0;
  int queens[SIZE];
  clock_t begin, end;
  double elapsed_secs;

  begin = clock();
  for (j = 0; j < SIZE/2; j++)
    count += 2*set_queen(0, j, queens);
  if (SIZE % 2)
    count += set_queen(0, SIZE/2, queens);
  end = clock();
  printf("%d\n", count);
  elapsed_secs = ((double)end - begin) / CLOCKS_PER_SEC;
  printf("elapsed time: %1.2f\n", elapsed_secs);
  return 0;
}
