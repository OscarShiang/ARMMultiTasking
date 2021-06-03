#include "common/print.h"
#include <string.h>

int fib(int n) {
  if (n <= 0)
    return 0;
  else if (n == 1)
    return 1;
  else
    return fib(n - 1) + fib(n - 2);
}

void worker(int argc, char* argv[]) {
  if (argc != 2) {
    printf("fib expects atarguments\n");
    return;
  }

  int num = 0;
  int len = strlen(argv[1]);
  for (int i = len - 1; i >= 0; i--) {
    num *= 10;
    num += (argv[1][i] - '0');
  }

  printf("%i\n", fib(num));
}
