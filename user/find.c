#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#define PATHBUFSIZ 512

char *fmtname(char *path) {
  char *p;

  // Find first character after last slash.
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  return p + 1;
}

void find(char *path, char *filename) {
  char path_buf[PATHBUFSIZ], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, 0)) < 0) {
    fprintf(stderr, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(stderr, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type) {
  case T_FILE: {
    // note: inode type = 2
    if (strcmp(fmtname(path), filename) == 0) {
      fprintf(stdout, "%s\n", path);
    }
    break;
  }

  case T_DIR: {
    // note: inode type = 1
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(path_buf)) {
      fprintf(stderr, "ls: path too long\n");
      break;
    }
    strcpy(path_buf, path);
    p = path_buf + strlen(path_buf);
    *p++ = '/'; // extending the tmp path as we explore the directory
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0 || strcmp(de.name, ".") == 0 ||
          strcmp(de.name, "..") == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = '\0';
      find(path_buf, filename);
    }
    break;
  }
  default: {
    // debug: what are other st.type(s) referring to?
    break;
  }
  }
  // close file descriptor after use
  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: find path name\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}