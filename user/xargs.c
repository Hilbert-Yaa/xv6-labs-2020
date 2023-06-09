#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#define BUFSIZ 32

struct execcmd {
  char *argv[MAXARG];
};

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "xargs: missing arguments\n");
    exit(1);
  }

  struct execcmd cmd;
  for (uint i = 0; i < argc - 1; ++i) {
    cmd.argv[i] = argv[i + 1];
  }

  char ch, arg_buf[BUFSIZ];
  uint8 cur = 0; // str cursor

  while (read(stdin, &ch, sizeof(char))) {
    fprintf(stderr, "current char: %c, buf strlen: %d\n", ch, strlen(arg_buf));
    if (ch == '\n') {
      // end of line, append buf to cmd args, exec cmd and reset buf ptr
      arg_buf[cur] = '\0';
      cmd.argv[argc - 1] = arg_buf;
      if (fork() > 0) {
        // parent: wait for child to finish, clear buf, reset buf ptr
        wait(NULL);
        cur = 0;
        arg_buf[cur] = '\0';
      } else {
        // child: exec cmd
        exec(cmd.argv[0], cmd.argv);
      }
    } else {
      // append ch to current s
      arg_buf[cur++] = ch;
    }
  }
  exit(0);
}