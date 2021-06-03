#include "common/assert.h"
#include "common/print.h"
#include "user/file.h"

void worker(int argc, char* argv[]) {
  if (argc != 2) {
    printf("cat requires 2 arguments\n");
    return;
  }

  char buf[513];
  int fd = open(argv[1], O_RDONLY);
  assert(fd >= 0);

  int ret;
  while ((ret = read(fd, buf, sizeof(buf)))) {
    buf[ret] = '\0';
    printf("%s", buf);
  }
  close(fd);
}
