#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Macros */
#define TOKEN_COMMENT '-'

/* Structures */
typedef struct {
    char *content;
    size_t length;
    int type;
} StructToken;

/* Variables */
static char *raw;
static size_t TokensCount = 0;
static size_t TokensCapacity = 0;
static StructToken *Tokens = NULL;
static StructToken token;

/* Functions */
static void FileReader(char *file);
static void FileTokenizer();
static void FileAnalyzer();

int main(int argc, char *argv[]){

  if (argc != 2) {
    (void) fputs("usage: pl0c file.pl0\n", stderr);
    exit(1);
  }

  FileReader(argv[1]);
  FileTokenizer();
  FileAnalyzer();

  return 0;
}


static void FileAnalyzer(){
  for(int i = 0; i < TokensCount; i++){
    if(Tokens[i].length > 0){
      printf("%d\t%ld\t%s\n",i,Tokens[i].length,Tokens[i].content);  
    }
  }
}

/*
 * Function: FileReader
 * Opens a file an load in memory its contents.
 */

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


static StructToken HandleSymbol(){

  token.length = 1;
  token.content = malloc(token.length);
  memcpy(token.content, raw, token.length);
  token.content[token.length] = '\0';
  raw++;
  
  return token;
}

static StructToken HandleText(){
  char *token_start = raw;

  while (isalnum(*raw) || *raw == '_'){
    raw++;
  }
  
  token.length = raw - token_start;
  token.content = malloc(token.length+1);
  memcpy(token.content, token_start, token.length);
  token.content[token.length] = '\0';
  
  return token;
}


void AddToken(StructToken token){
    if(TokensCount >= TokensCapacity){
        TokensCapacity = TokensCapacity == 0 ? 20 : TokensCapacity + 1;
        Tokens = realloc(Tokens, TokensCapacity * sizeof(StructToken));
    }
    Tokens[TokensCount++] = token;
}


static void FileTokenizer(){
  StructToken token;
  
  while(*raw != EOF){
   
    if (*raw == TOKEN_COMMENT){
      HandleComment();
    }
    
    if(*raw == '\n' || *raw == ' '){
      raw++;
    }else if (isalnum(*raw)){
      token = HandleText();
      AddToken(token);
    }else{
      token = HandleSymbol();
      AddToken(token);
    }
  }
}


