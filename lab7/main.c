#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define ROOT 0
#define SIZE 16

#define STATUS_WORK 1
#define STATUS_END -1

#define INIT_DEPTH 2
#define DEPTH 4

#define FALSE 0
#define TRUE 1


int check(int ridx, int cidx, int *queens){
  /**
  Проверка возможности размещения ферзя в позиции (ridx, cidx)
  */
  int i, delta;

  for (i = 0; i < ridx; i++){
    if (queens[i] == cidx) {
      /* Проверяем столбцы */
      return FALSE;
    }
    
    delta = ridx - i;
    if (queens[i] - delta == cidx || queens[i] + delta == cidx) {
      /* Проверяем диагонали */
      return FALSE;
    }
  }

  return TRUE;
}


void print_queens(int *queens, int max_ridx, char *header) {
  /**
  Напечатать доску до строки mx_ridx
  */
  int i, j;
  char msg[1024], *buf;

  buf = msg;
  buf += sprintf(buf, "%s\n", header);
  for (i = 0; i < max_ridx; i++) {
    for (j = 0; j < SIZE; j++) {
      if (j == queens[i])
        buf += sprintf(buf, "1");
      else
        buf += sprintf(buf, "0");
    }
    buf += sprintf(buf, "\t%d\n", queens[i]);
  }
  printf("%s", msg);
}


long int set_queen(int ridx, int cidx, int *queens){
  /**
  Ставим ферзя в позицию (ridx, cidx)
  */
  int j;
  long long int count = 0;

  queens[ridx] = cidx;

  if (ridx == SIZE - 1){
    /* Нашли решение */
    return 1;
  }

  /* Пытаемся найти позицию в строке сверху и поставить туда ферзя */
  for (j = 0; j < SIZE; j++){
    if (check(ridx + 1, j, queens))
      count += set_queen(ridx + 1, j, queens);
  }

  return count;
}

void build_msg(int status, int ridx, int cidx, int *queens, int *msg) {
  /**
  Собираем сообщение для передачи работнику
  */
  msg[0] = status;
  msg[1] = ridx;
  msg[2] = cidx;
  int i;
  for (i = 0; i < SIZE; i++) {
    (msg+3)[i] = queens[i];
  }
}


int walk_in_row(int *queens, int ridx, int cidx) {
  /**
  Поиск первой подходящей позиции в строке, начиная с cidx
  */

  if (cidx > SIZE || cidx < 0) {
    /* не больше вариантов, которые можно былы бы рассмотреть */
    return FALSE;
  }

  int j;
  /* Ищем подходящую позицию */
  for (j = cidx; j < SIZE; j++) {
    if (check(ridx, j, queens)) {
      queens[ridx] = j;
      return TRUE;
    }
  }

  return FALSE;
}


void clear_queens(int *queens, int ridx) {
  int i;
  for (i = ridx; i < SIZE; i++)
    queens[i] = 0;
}


int walk(int *queens, int ridx, int cidx, int depth) {
  /**
  Поиск следующей позиции, с которой можно начать поиск работнику
  */
  int flg;
  char buf[1024];

  while (TRUE) {
    flg = FALSE;

    if (ridx < depth) {
      /* Пробуем найти подходящую позицию сверху */
      flg = walk_in_row(queens, ridx + 1, 0);
      while (flg) {
        ridx++;
        if (ridx == depth) {
          /* Искомая позиция найдена */
          return TRUE;
        }
        flg = walk_in_row(queens, ridx + 1, 0);
      }
    }

    /* Пробуем найти следующую подходящую позицию в строке */
    flg = walk_in_row(queens, ridx, queens[ridx] + 1);
    if (flg && ridx == depth) {
      /* Искомая позиция найдена */
      return TRUE;
    }

    /* Пробуем найти подходящую позицию снизу */
    while (!flg) {
      if (ridx == 0) {
        /* Далее больше вниз шагать нельзя */
        return FALSE;
      }
      flg = walk_in_row(queens, ridx - 1, queens[ridx - 1] + 1);
      ridx--;
    }
  }
  return FALSE;
}


int **init_messages(int count) {
  int **messages, i;
  messages = malloc(sizeof(int *) * (count - 1));
  for (i = 0; i < (count - 1); i++)
    messages[i] = malloc(sizeof(int) * (SIZE + 3));
  return messages;
}

void dispatch() {
  int i, size, flg;
  int ridx, cidx;
  long long int gcount = 0, count;
  int sended = 0, received = 0;
  int **messages;
  int queens[SIZE];
  MPI_Status status;
  MPI_Request request;

  MPI_Comm_size(MPI_COMM_WORLD, &size);

  messages = init_messages(size);
  clear_queens(queens, 0);
  ridx = 0; cidx = 0;

  /* Сначала раздаем воркерам задания покрупнее */
  for (i = 1; i < size; i++) {
    flg = walk(queens, ridx, cidx, INIT_DEPTH);
    if (!flg)
      break;

    ridx = INIT_DEPTH;
    cidx = queens[INIT_DEPTH];
    build_msg(STATUS_WORK, ridx, cidx, queens, messages[i-1]);
    
    MPI_Isend(messages[i-1], SIZE + 3, MPI_INT,
	      i, 0, MPI_COMM_WORLD, &request);
    sended++;
  }

  /* Ищем следующую позицию на глубине INIT_DEPTH, чтобы от нее */
  /* начать раздавать более мелкие задания                      */
  flg = walk(queens, ridx, cidx, INIT_DEPTH);
  cidx = queens[INIT_DEPTH];
    
  /* Раздаем воркерам задания помельче и получаем от них результаты работы */
  while (flg || sended > received) {
    if (received < sended) {
      MPI_Recv(&count, 1, MPI_LONG_LONG_INT, MPI_ANY_SOURCE,
  	       0, MPI_COMM_WORLD, &status);
      received++;
      gcount += count;
    }

    if (flg) {
      flg = walk(queens, ridx, cidx, DEPTH);

      if (flg) {
        ridx = DEPTH;
        cidx = queens[DEPTH];
        build_msg(STATUS_WORK, ridx, cidx, queens,
                   messages[status.MPI_SOURCE-1]);
     
        MPI_Isend(messages[status.MPI_SOURCE-1], SIZE + 3, MPI_INT,
	  	  status.MPI_SOURCE, 0, MPI_COMM_WORLD, &request);
        sended++;
      }
    }
  }

  /* Сообщаем все воркерам об окончании работы */
  for (i = 1; i < size; i++) {
    build_msg(STATUS_END, DEPTH, queens[DEPTH], queens, messages[i-1]);
    MPI_Ssend(messages[i-1], SIZE + 3, MPI_INT, i, 0, MPI_COMM_WORLD);
  }

  printf("count = %d\n", gcount);
}


void work() {
  long long int count;
  int buf[SIZE + 3];
  MPI_Status status;
  MPI_Request request;

  while (TRUE) {
    MPI_Recv((void*) buf, SIZE + 3, MPI_INT, ROOT, 0, MPI_COMM_WORLD, &status);

    if (buf[0] == STATUS_END)
      break;

    count = set_queen(buf[1], buf[2], buf + 3);
    MPI_Isend(&count, 1, MPI_LONG_LONG_INT, ROOT, 0, MPI_COMM_WORLD, &request);
  }
}


int main(int argc,char *argv[]){
  int rank, size;
  double start;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == ROOT) {
    start = MPI_Wtime();
    dispatch();
    printf("elapsed time: %1.2f\n", MPI_Wtime() - start);
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    printf("processor count: %d, matrix size: %dx%d\n", size, SIZE, SIZE);
  } else {
    work();
  }

  MPI_Finalize();
  return 0;
}
