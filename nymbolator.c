#include <sys/stat.h>

#include <ctype.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef DEBUG  
#define DEBUG(...) printf(__VA_ARGS__)
#else
#define DEBUG(...) do {} while (0)
#endif


#define TOKEN_LIBRARY "library"
#define TOKEN_USE "use"
#define TOKEN_ALL "all"
#define TOKEN_ENTITY "entity"
#define TOKEN_TYPE "type"
#define TOKEN_SIGNAL "signal"
#define TOKEN_FUNCTION "function"
#define TOKEN_VARIABLE "variable"
#define TOKEN_RANGE "range"
#define TOKEN_TO "to"
#define TOKEN_DOWNTO "downto"

#define TOKEN_STD_LOGIC_VECTOR "std_logic_vector"
#define TOKEN_INTEGER "integer"

#define TOKEN_FOR "for"
#define TOKEN_IN "in"
#define TOKEN_LOOP "loop"

#define TOKEN_GENERIC "generic"
#define TOKEN_PORT "port"

#define TOKEN_IN "in"
#define TOKEN_OUT "out"


#define TOKEN_ARCHITECTURE "architecture"

#define TOKEN_BEGIN "begin"
#define TOKEN_IS "is"
#define TOKEN_OF "of"
#define TOKEN_END "end"
#define TOKEN_COMMENT "-"

#define TOKEN_DOT "."
#define TOKEN_EQUAL "="
#define TOKEN_COMMA ","
#define TOKEN_SEMICOLON	";"
#define TOKEN_ASSIGN ":"
#define TOKEN_HASH "#"
#define TOKEN_LESSTHAN "<"
#define TOKEN_GREATERTHAN ">"
#define TOKEN_PLUS "+"
#define TOKEN_MINUS "-"
#define TOKEN_MULTIPLY "*"
#define TOKEN_DIVIDE "/"
#define TOKEN_LPAREN "("
#define TOKEN_RPAREN ")"


const char *keywords[] = {
  TOKEN_ENTITY,
  TOKEN_GENERIC,
  TOKEN_PORT,
  TOKEN_IN,
  TOKEN_OUT,
  TOKEN_ARCHITECTURE,
  TOKEN_BEGIN,
  TOKEN_IS,
  TOKEN_OF,
  TOKEN_END,
  TOKEN_COMMENT,
  TOKEN_ALL,
  TOKEN_LIBRARY,
  TOKEN_USE,
  TOKEN_TYPE,
  TOKEN_SIGNAL,
  TOKEN_VARIABLE,
  TOKEN_FUNCTION,
  NULL  // Marcador final
};

const char *keywords_type[] = {
  "FINISH",
  "SYMBOL",
  "KEYWORD",
  "NUMBER",
  "IDENTIFIER",
  "UNKOWN"
};

static char *raw;
static int type;
static size_t line = 1;

enum tokenType {FINISH, SYMBOL, WORD, NUMBER, IDENTIFIER, UNKOWN};

struct tokenStruct {  
  char *content;  
  enum tokenType type;
  size_t length;
};

struct tokenStruct *token;

void CleanWhiteSpace(void){
  while (*raw == ' ' || *raw == '\t' || *raw == '\n') {
    if (*raw++ == '\n'){
      ++line;
    }
  }
}

static void error(const char *fmt, ...){
  va_list ap;

  (void) fprintf(stderr, "error: %lu: ", line);

  va_start(ap, fmt);
  (void) vfprintf(stderr, fmt, ap);
  va_end(ap);

  (void) fputc('\n', stderr);

  exit(1);
}

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

static void comment(void) {
  int ch;
  while ((ch = *raw++) != '\n') {
    if (ch == '\0')
      error("unterminated comment");
  }
  ++line;
}

static int identification(void) {
  enum tokenType type = WORD; 
  char *p;
  size_t i, len;

  p = raw;
  while (isalnum(*raw) || *raw == '_')
    ++raw;

  len = raw - p;

  --raw;

  free(token);          

  token = malloc(sizeof(struct tokenStruct));
  if (token == NULL) error("malloc failed");
 
  token->content = malloc(len + 1);  // +1 para el '\0'
  if (token->content == NULL) {
    error("malloc failed para content");
  }
  
  token->length= len+1;
  token->type = WORD;
  
  for (i = 0; i < len; i++){
    token->content[i] = *p++;
  }
    
  token->content[i] = '\0';

  for (const char **kw = keywords; *kw != NULL; kw++) {
    if (strcasecmp(token->content, *kw) == 0) {
      return type;
    }
  }

  type = IDENTIFIER;
  token->type = type;
  
  return type;
}

static int number(void){
  enum tokenType type = NUMBER; 
  const char *errstr;
  char *p;
  size_t i, j = 0, len;

  p = raw;
  while (isdigit(*raw) || *raw == '_')
    ++raw;

  len = raw - p;

  --raw;

  free(token);          

  token = malloc(sizeof(struct tokenStruct));
  if (token == NULL) error("malloc failed");
 
  token->content = malloc(len + 1);  // +1 para el '\0'
  if (token->content == NULL) {
    error("malloc failed para content");
  }
  
  token->length= len+1;
  token->type = NUMBER;

  for (i = 0; i < len; i++) {
    if (isdigit(*p))
      token->content[j++] = *p;
    ++p;
  }
  token->content[j] = '\0';

  (void) strtonum(token->content, 0, LONG_MAX, &errstr);
  if (errstr != NULL)
    error("invalid number: %s", token);

  return type;
}

/* RULES */
/* Inician siempre con una letra (A–Z, a–z). */

/* Pueden contener letras, dígitos (0–9) y guiones bajos _. */

/* No pueden empezar ni terminar con _. */

/* No se permiten dos guiones bajos consecutivos (ni __). */

/* No pueden usar palabras reservadas de VHDL. */

/* No distinguen mayúsculas/minúsculas (case‑insensitive). */

/* Pueden tener cualquier longitud dentro de una única línea. */


static int lex(void){
  enum tokenType type = SYMBOL; 
  again:
  CleanWhiteSpace();

  if (isalpha(*raw))
    return identification();

  if (isdigit(*raw))
    return number();

  switch (*raw) {
  case '-':
    if(*raw++ != '-'){
      return identification();
    }
    comment();
    goto again;
  case '.':
  case '=':
  case '\'':
  case '"':
  case ',':
  case ';':
  case '#':
  case '<':
  case '>':
  case '+':
  case '*':
  case '/':
  case '(':
  case ')':
    token->content[0] = raw[0];
    token->content[1] = '\0';
    token->length = 2;
    token->type = SYMBOL;
    break;
  case ':':
    raw++;
    if(*raw == '='){
      raw--;
      token->content[0] = raw[0];
      token->content[1] = raw[1];
      token->content[2] = raw[2];
      token->content[3] = '\0';      
      token->length = 3;
      token->type = WORD;
      raw = raw + 2;
    }else{
      raw--;
      token->content[0] = raw[0];
      token->content[1] = '\0';
      token->length = 2;
      token->type = SYMBOL;
    }
    break;
  case '\0':
    type = FINISH;
    break;
  default:
    error("unknown token: '%c'", *raw);
  }

  return type;
}

static void parse(void){
  while ((type = lex()) != 0) {
    ++raw;
    (void) fprintf(stdout, "%lu|%s|\t", line, keywords_type[type]);

    if((strcasecmp(token->content, TOKEN_ENTITY) == 0)){
      ++raw;
      if((strcasecmp(keywords_type[type], "IDENTIFIER") == 0)){
	++raw;
	if((strcasecmp(token->content, TOKEN_IS) == 0)){
	  printf("Ok!");
	}
      }
    }
    
    if(type == WORD || type == IDENTIFIER ||type == NUMBER){
      (void)fputs(token->content, stdout);
    }else if (type == SYMBOL){
      (void)fputs(token->content, stdout);
    }
    
    (void) fputc('\n', stdout);
  }
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


/* void parse_design_file() { */
/*     while(type != EOF) { */
/*         switch(type) { */
/*             case TOK_ENTITY: parse_entity(); break; */
/*             case TOK_ARCHITECTURE: parse_architecture(); break; */
/*             case TOK_PACKAGE: parse_package(); break; */
/*             // ... otros componentes */
/*         } */
/*     } */
/* } */
