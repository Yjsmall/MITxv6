#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1

/**
 * @brief Prints the first element from the pipe and sets it to first
 * @param datas The pipe array
 * @param first Pointer to the variable that stores the first element
 * @return 0 on success, -1 on failure
 */
int printfirstelem(int datas[2], int *first) {
  if (read(datas[RD], first, sizeof(int)) == sizeof(int)) {
    printf("prime %d\n", *first);
    return 0;
  }
  return -1;
}

/**
 * @brief Filters data, writing non-multiples of first to the new pipe
 * @param raw Input data pipe array
 * @param datas Output data pipe array
 * @param first Prime number used for filtering
 */
void fliterdata(int raw[2], int datas[2], int first) {
  int data;
  while (read(raw[RD], &data, sizeof(int)) != 0) {
    if (data % first != 0) {
      write(datas[WR], &data, sizeof(int));
    }
  }
  close(raw[RD]);
  close(datas[WR]);
}

/**
 * @brief Recursively processes the prime logic
 * @param raw Input data pipe array
 */
void prime(int raw[2]) {
  close(raw[WR]);

  int first;
  if (printfirstelem(raw, &first) == 0) {
    int datas[2];
    pipe(datas);
    fliterdata(raw, datas, first);

    int pid = fork();
    if (pid == 0) { // Child process
      prime(datas);
    } else if (pid > 0) { // Parent process
      close(datas[RD]);
      wait(0);
    } else {
      exit(1);
    }
  }

  // TODO: Why here con't use exit(0)?
  return;
}

int main(int argc, char *argv[]) {
  int datas[2];
  pipe(datas);

  for (int i = 2; i <= 35; i++) {
    write(datas[WR], &i, sizeof(int));
  }
  close(datas[WR]); // Close the write end

  int pid = fork();
  if (pid == 0) {
    prime(datas);
  } else if (pid > 0) {
    close(datas[RD]);
    wait(0);
  } else {
    exit(1);
  }
  exit(0);
}