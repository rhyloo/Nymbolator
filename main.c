#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TOKEN_COMMENT '-'

static char *raw;

static void FileReader(char *file){
  int file_descriptor;
  struct stat stats;

  file_descriptor = open(file, O_RDONLY, 0);
  fstat(file_descriptor, &stats);
  raw = malloc(stats.st_size + 1);
  read(file_descriptor, raw, stats.st_size);
  raw[stats.st_size] = EOF;
  (void) close(file_descriptor);
}

static void FileParsing(){
  while(*raw != EOF){
    printf("%c\n",*raw);
    if (*raw == TOKEN_COMMENT){
      printf("Comment detected!\n");
    }
    *raw++;
  }
}

int main(int argc, char *argv[]){

  if (argc != 2) {
    (void) fputs("usage: pl0c file.pl0\n", stderr);
    exit(1);
  }

  FileReader(argv[1]);
  FileParsing();

  return 0;
}
