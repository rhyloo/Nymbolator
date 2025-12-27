#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tokens.h"


#ifdef DEBUG  
#define DEBUG(...) printf(__VA_ARGS__)
#else
#define DEBUG(...) do {} while (0)
#endif

static void readin(char *file){
  int fd;
  struct stat st;
  char *p;

  DEBUG("Nombre del archivo: %s\n",file);
  DEBUG("Extensión: %s\n",strrchr(file, '.'));
    
  if (strrchr(file, '.') == NULL)
    error("file must end in '.vhdl'");
  
  if (!!strcmp(strrchr(file, '.'), ".vhdl"))
    error("file must end in '.vhdl'");

  if ((fd = open(file, O_RDONLY)) == -1)
    error("couldn't open %s", file);

  if (fstat(fd, &st) == -1)
    error("couldn't get file size");

  if ((raw = malloc(st.st_size + 1)) == NULL)
    error("malloc failed");

  if (read(fd, raw, st.st_size) != st.st_size)
    error("couldn't read %s", file);
  raw[st.st_size] = '\0';

  (void) close(fd);
}

int main(int argc, char *argv[]){
  char *startp;

  if (argc != 2) {
    (void) fputs("usage: pl0c file.pl0\n", stderr);
    exit(1);
  }

  readin(argv[1]);
  startp = raw;

  parse();

  free(startp);

  return 0;
}
