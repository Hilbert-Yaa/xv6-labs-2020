#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  if (argc != 1) {
    fprintf(2, "Usage: pingpong\n");
    exit(1);
  }
  int p[2];
  if (pipe(p) < 0) {
    fprintf(2, "pipe: failed to create pipe\n");
    exit(1);
  }
  if (write(p[1], "x", 1) != 1) {
    fprintf(2, "write: failed to write to pipe\n");
  };
  int stat;
  if (fork() == 0) {
    if (read(p[0], 0, 1) != 1) {
      fprintf(2, "read: failed to read from pipe\n");
      exit(1);
    }
    fprintf(1, "%d: received ping\n", getpid());
    write(p[1], "x", 1);
  } else {
    wait(&stat);
    if (stat < 0) {
      fprintf(2, "wait: child process exit with code %d\n", stat);
      exit(1);
    }
    if (read(p[0], 0, 1) != 1) {
      fprintf(2, "read: failed to read from pipe\n");
      exit(1);
    }
    fprintf(1, "%d: received pong\n", getpid());
  }
  exit(0);
}