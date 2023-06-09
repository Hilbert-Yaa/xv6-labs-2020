#include "kernel/types.h"
#include "user/user.h"

int main (int argc, char* argv[]) {
  if (argc != 1) {
    fprintf(stderr, "Usage: uptime\n");
    exit(1);
  }
  int t = uptime();
  if (t < 0) {
    fprintf(stderr, "uptime: failed to get uptime\n");
    exit(1);
  }
  fprintf(stdout, "%d ticks up\n", t);
  exit(0);
}