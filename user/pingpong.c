#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  if (argc != 1) {
    fprintf(stderr, "Usage: pingpong\n");
    exit(1);
  }
  int p[2];
  if (pipe(p) < 0) {
    fprintf(stderr, "pipe: failed to create pipe\n");
    exit(1);
  }
  if (write(p[PIPE_WRITE], "x", 1) != 1) {
    fprintf(stderr, "write: failed to write to pipe\n");
  };
  int stat;
  if (fork() == 0) {
    if (read(p[PIPE_READ], 0, 1) != 1) {
      fprintf(stderr, "read: failed to read from pipe\n");
      exit(1);
    }
    fprintf(stdout, "%d: received ping\n", getpid());
    write(p[PIPE_WRITE], "x", 1);
  } else {
    wait(&stat);
    if (stat < 0) {
      fprintf(stderr, "wait: child process exit with code %d\n", stat);
      exit(1);
    }
    if (read(p[PIPE_READ], 0, 1) != 1) {
      fprintf(stderr, "read: failed to read from pipe\n");
      exit(1);
    }
    fprintf(stdout, "%d: received pong\n", getpid());
  }
  exit(0);
}