#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char const *argv[]) {
  if (argc != 2) { //参数错误
    fprintf(2, "usage: sleep <time>\n");
    exit(1);
  }
  int n = atoi(argv[1]);
  sleep(n);
  exit(0);
}
