#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"
#define BUFSIZ 32

struct execcmd {
  char *argv[MAXARG];
};

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: time cmd...\n");
    exit(1);
  }

  struct execcmd cmd;
  for (uint i = 0; i < argc - 1; ++i) {
    cmd.argv[i] = argv[i + 1];
  }

  int t_start = uptime();
  if (fork() == 0) {
    exec(cmd.argv[0], cmd.argv);
  }
  wait(NULL);
  int t_end = uptime();
  fprintf(stderr, "%s: %d ticks\n", cmd.argv[0], t_end - t_start);

  exit(0);
}
