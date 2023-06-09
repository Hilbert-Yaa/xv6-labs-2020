#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#define PATHBUFSIZ 512

// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9.

int matchhere(char*, char*);
int matchstar(int, char*, char*);
int match(char*, char*);

char *fmtname(char *path) {
  char *p;

  // Find first character after last slash.
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  return p + 1;
}

void find(char *path, char *pattern) {
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
    if (match(pattern, fmtname(path))) {
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
      find(path_buf, pattern);
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

int
match(char *re, char *text)
{
  if(re[0] == '^')
    return matchhere(re+1, text);
  do{  // must look at empty string
    if(matchhere(re, text))
      return 1;
  }while(*text++ != '\0');
  return 0;
}

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text)
{
  if(re[0] == '\0')
    return 1;
  if(re[1] == '*')
    return matchstar(re[0], re+2, text);
  if(re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  if(*text!='\0' && (re[0]=='.' || re[0]==*text))
    return matchhere(re+1, text+1);
  return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
  do{  // a * matches zero or more instances
    if(matchhere(re, text))
      return 1;
  }while(*text!='\0' && (*text++==c || c=='.'));
  return 0;
}
