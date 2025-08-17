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

typedef struct {
    char *content;
    size_t length;
    int type;
} StructToken;

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

static void HandleComment(){
  *raw++;
  if(*raw == TOKEN_COMMENT){
    while(*raw != '\n'){
      *raw++;
    }
    *raw++;
  }{
    *raw--;
  }
}

static void HandleSymbol(){
  StructToken token;
  token.length = 1;
  token.content = malloc(token.length);
  memcpy(token.content, raw, token.length);
  token.content[token.length] = '\0'; // añadir terminador
  printf("%s\n",token.content);
  *raw++;
}

static void HandleText(){
  char *token_start = raw;
  StructToken token;
  
  while (isalnum(*raw) || *raw == '_'){
    *raw++;
  }
  
  token.length = raw - token_start;
  token.content = malloc(token.length);
  memcpy(token.content, token_start, token.length);
  token.content[token.length] = '\0'; // añadir terminador
  printf("%s\n",token.content);
  *raw--;
}


static void FileParsing(){
  while(*raw != EOF){
   
    if (*raw == TOKEN_COMMENT){
      HandleComment();
    }
    
    if(*raw == '\n' || *raw == ' '){
      *raw++;
    }else if (isalnum(*raw)){
      HandleText();
      *raw++;
    }else{
      HandleSymbol();
    }
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
