#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

__attribute((noreturn))
void sieve_handler(int prev[2]) {
  // read-only to prev pipe, write-only to next pipe
  close(prev[PIPE_WRITE]);

  uint8 p;

  int stat = read(prev[PIPE_READ], &p, sizeof(p));
  if (stat == 0) {
    // end of work, close pipe and exit
    close(prev[PIPE_READ]);
    exit(0);
  } else if (stat != sizeof(p)) {
    fprintf(stderr, "read: failed to read from prev pipe\n");
    exit(1);
  }

  fprintf(stdout, "prime %d\n", p);

  int next[2];
  if (pipe(next) < 0) {
    fprintf(stderr, "pipe: failed to create next pipe\n");
    exit(1);
  }

  int pid = fork();
  if (pid < 0) {
    fprintf(stderr, "fork: failed to create child process\n");
    close(prev[PIPE_READ]);
    close(next[PIPE_READ]);
    close(next[PIPE_WRITE]);
    exit(1);
  } else if (pid > 0) {
    // parent: write-only to next pipe, sieve nums from prev into next pipe
    close(next[PIPE_READ]);
    uint8 n;
    while(read(prev[PIPE_READ], &n, sizeof(n)) == sizeof(n)) {
      if (n % p != 0) {
        if (write(next[PIPE_WRITE], &n, sizeof(n)) != sizeof(n)) {
          fprintf(stderr, "write: failed to write to next pipe\n");
          exit(1);
        }
      }
    }
    close(next[PIPE_WRITE]);
    wait(NULL);
  } else {
    // child: invoke the sieve_handler with next pipe
    close(prev[PIPE_READ]);
    sieve_handler(next);
  }
  exit(0);
}

int main(int argc, char *argv[]) {
  if (argc != 1) {
    fprintf(stderr, "Usage: primes\n");
    exit(1);
  }
  int p[2];

  if (pipe(p) < 0) {
    fprintf(stderr, "pipe: failed to create root pipe\n");
    exit(1);
  }

  int pid = fork();

  if (pid < 0) {
    fprintf(stderr, "fork: failed to create child process\n");
    exit(1);
  } else if (pid > 0) {
    // parent: write-only, send nums 2..35 to pipe, wait for child
    close(p[PIPE_READ]);
    for (uint8 i = 2; i < 35; ++i) {
      if (write(p[PIPE_WRITE], &i, sizeof(i)) != sizeof(i)) {
        fprintf(stderr, "write: failed to write to root pipe\n");
        exit(1);
      }
    }
    close(p[PIPE_WRITE]);
    wait(NULL);
  } else {
    // child: invoke the sieve_handler
    sieve_handler(p);
  }
  exit(0);
}