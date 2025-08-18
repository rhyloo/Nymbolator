/* Macros */
#define TOKEN_COMMENT '-'
#define TOKEN_SEMICOLON ";"
#define TOKEN_ENTITY "ENTITY"
#define TOKEN_END "END"

/* Structures */
typedef struct {
  char *content;
  size_t length;
  int type;
} StructToken;

/* Variables */
char *raw;
size_t TokensCount = 0;
size_t TokensCapacity = 0;
StructToken *Tokens = NULL;
StructToken token;

/* Functions */
void FileReader(char *file);
void FileTokenizer();
void FileAnalyzer();


/* Option C: Hash table or trie (for large language parsing)
   
   If you’re eventually building a full VHDL parser, a hash table or
   trie lookup for keywords is the industry-standard approach. For
   small files, Option B is usually enough. */
/* Keyword enum */
typedef enum {
  KW_UNKNOWN,
  KW_LIBRARY,
  KW_USE,
  KW_ALL,
  KW_ENTITY,
  KW_TYPE,
  KW_SIGNAL,
  KW_FUNCTION,
  KW_VARIABLE,
  KW_RANGE,
  KW_TO,
  KW_DOWNTO,
  KW_STD_LOGIC_VECTOR,
  KW_INTEGER,
  KW_FOR,
  KW_IN,
  KW_LOOP,
  KW_GENERIC,
  KW_PORT,
  KW_OUT,
  KW_ARCHITECTURE,
  KW_BEGIN,
  KW_IS,
  KW_OF,
  KW_END,
  KW_COMMENT,
  KW_DOT,
  KW_EQUAL,
  KW_COMMA,
  KW_SEMICOLON,
  KW_ASSIGN,
  KW_HASH,
  KW_LESSTHAN,
  KW_GREATERTHAN,
  KW_PLUS,
  KW_MINUS,
  KW_MULTIPLY,
  KW_DIVIDE,
  KW_LPAREN,
  KW_RPAREN,
  KW_POSITIVE,
  KW_STD_ULOGIC,
  KW_STD_ULOGIC_VECTOR,
  KW_NATURAL,
  KW_BOOLEAN,
  KW_UNSIGNED,
  KW_STD_LOGIC
} KeywordType;

/* Keyword mapping table */
typedef struct {
  const char *str;
  KeywordType type;
} KeywordMap;

KeywordMap keyword_table[] = {
  {"library", KW_LIBRARY},
  {"use", KW_USE},
  {"all", KW_ALL},
  {TOKEN_ENTITY, KW_ENTITY},
  {"type", KW_TYPE},
  {"signal", KW_SIGNAL},
  {"function", KW_FUNCTION},
  {"variable", KW_VARIABLE},
  {"range", KW_RANGE},
  {"to", KW_TO},
  {"downto", KW_DOWNTO},
  {"std_logic_vector", KW_STD_LOGIC_VECTOR},
  {"integer", KW_INTEGER},
  {"for", KW_FOR},
  {"in", KW_IN},
  {"loop", KW_LOOP},
  {"generic", KW_GENERIC},
  {"port", KW_PORT},
  {"out", KW_OUT},
  {"architecture", KW_ARCHITECTURE},
  {"begin", KW_BEGIN},
  {"is", KW_IS},
  {"of", KW_OF},
  {TOKEN_END, KW_END},
  {"-", KW_COMMENT},
  {".", KW_DOT},
  {"=", KW_EQUAL},
  {",", KW_COMMA},
  {TOKEN_SEMICOLON, KW_SEMICOLON},
  {":", KW_ASSIGN},
  {"#", KW_HASH},
  {"<", KW_LESSTHAN},
  {">", KW_GREATERTHAN},
  {"+", KW_PLUS},
  {"-", KW_MINUS},
  {"*", KW_MULTIPLY},
  {"/", KW_DIVIDE},
  {"(", KW_LPAREN},
  {")", KW_RPAREN},
  {"positive", KW_POSITIVE},
  {"std_ulogic", KW_STD_ULOGIC},
  {"std_ulogic_vector", KW_STD_ULOGIC_VECTOR},
  {"natural", KW_NATURAL},
  {"boolean", KW_BOOLEAN},
  {"unsigned", KW_UNSIGNED},
  {"std_logic", KW_STD_LOGIC}
};

static KeywordType GetKeywordType(const char *token) {
  for (int i = 0; i < sizeof(keyword_table)/sizeof(keyword_table[0]); i++) {
    if (strcasecmp(token, keyword_table[i].str) == 0)
      return keyword_table[i].type;
  }
  return KW_UNKNOWN;
}

const char* keyword_to_string(KeywordType type) {
    size_t table_size = sizeof(keyword_table) / sizeof(keyword_table[0]);
    for (size_t i = 0; i < table_size; i++) {
        if (keyword_table[i].type == type) {
            return keyword_table[i].str;
        }
    }
    return "UNKNOWN";
}



typedef struct {
  char *name;
  KeywordType type;  // e.g. KW_POSITIVE, KW_INTEGER, ...
} StructGenerics;

typedef enum {
  PM_IN, PM_OUT, PM_INOUT, PM_BUFFER 
} PortMode;  

typedef struct {
  char *name;
  PortMode mode;
  KeywordType type; // type of port signal (KW_STD_ULOGIC, KW_INTEGER, etc.)
} StructPorts;

typedef struct {
  char *name;
  StructGenerics *generics;
  size_t genericsCount;
  StructPorts *ports;
  size_t portsCount;
} StructEntity;


#define PAUSE()            \
    do {                       \
        int c;                 \
        do {                   \
            c = getchar();     \
        } while (c != '\n' && c != EOF); \
    } while (0)


static int TokenNumber = 0;
#define GETKEYWORD(position) GetKeywordType(Tokens[TokenNumber-position].content)
#define AVOIDOF() TokenNumber < TokensCount
#define KWS(text) keyword_to_string(text)

StructEntity entity = {};


static void HandleEntity() {
 
  TokenNumber++; 
  entity.name = Tokens[TokenNumber].content;
  TokenNumber=+2;
  
  while (AVOIDOF()) {
    /* printf("%s\n",Tokens[TokenNumber].content); */
    if(TokenNumber > 2){
      if(GETKEYWORD(0) == KW_END && GETKEYWORD(-1) == KW_ENTITY && GETKEYWORD(-2) == KW_SEMICOLON){
	TokenNumber++;
	break;
      }else if (GETKEYWORD(0) == KW_END && GETKEYWORD(-1) == KW_ENTITY &&  strcmp(Tokens[TokenNumber+2].content, entity.name) == 0 && GETKEYWORD(-3) == KW_SEMICOLON){
	TokenNumber = TokenNumber + 2;
	break;
      }else if (GETKEYWORD(0) == KW_END &&  strcmp(Tokens[TokenNumber+1].content, entity.name) == 0 && GETKEYWORD(-2) == KW_SEMICOLON){
	break;
      }
    }

    if (GETKEYWORD(0) == KW_GENERIC) {
      TokenNumber = TokenNumber+2;
      while (AVOIDOF()) {
	
	if(TokenNumber > 2){
	  if(GETKEYWORD(1) == KW_SEMICOLON && GETKEYWORD(2) == KW_RPAREN && GETKEYWORD(3) != KW_RPAREN){
	    TokenNumber--;
	    break;
	  }
	}
	
	entity.generics = realloc(entity.generics, (entity.genericsCount+1)*sizeof(StructGenerics));
	entity.generics[entity.genericsCount].name = Tokens[TokenNumber].content;
	TokenNumber=TokenNumber+2;
	entity.generics[entity.genericsCount].type = GETKEYWORD(0);
	entity.genericsCount++;	

	if (GETKEYWORD(0) == KW_SEMICOLON){
	    TokenNumber++;
	}else{
	  while(GETKEYWORD(0) != KW_SEMICOLON){
	    TokenNumber++;
	  }	  
	}
	TokenNumber++;
      }
    }else if (GETKEYWORD(0) == KW_PORT) {
      printf("%s\n",Tokens[TokenNumber].content);
      TokenNumber = TokenNumber+2;
      printf("%s\n",Tokens[TokenNumber].content);
      
      while (AVOIDOF()) {

	if(TokenNumber > 3){
	  if(GETKEYWORD(1) == KW_SEMICOLON && GETKEYWORD(2) == KW_RPAREN && GETKEYWORD(0) == KW_END){
	    TokenNumber--;
	    break;
	  }
	}
	entity.ports = realloc(entity.ports, (entity.portsCount+1)*sizeof(StructPorts));
		
	entity.ports[entity.portsCount].name = Tokens[TokenNumber].content;	
	TokenNumber = TokenNumber + 2;
	if (!strcmp(Tokens[TokenNumber].content,"in")){
	  entity.ports[entity.portsCount].mode = PM_IN;
	}else if (!strcmp(Tokens[TokenNumber].content,"out")){
	  entity.ports[entity.portsCount].mode = PM_OUT;
	}else if (!strcmp(Tokens[TokenNumber].content,"inout")){
	  entity.ports[entity.portsCount].mode = PM_INOUT;
	}else {
	  entity.ports[entity.portsCount].mode = PM_BUFFER;
	}

	TokenNumber++; 
	entity.ports[entity.portsCount].type = GETKEYWORD(0);
	entity.portsCount++;

	if (GETKEYWORD(0) == KW_SEMICOLON){
	    TokenNumber++;
	}else{
	  while(GETKEYWORD(0) != KW_SEMICOLON){
	    TokenNumber++;
	  }	  
	}
	TokenNumber++;
      }
    }
    TokenNumber++;
  }
  printf("Entity %s parsed: %zu generics, %zu ports\n",entity.name, entity.genericsCount,entity.portsCount);
  for (size_t i=0; i<entity.genericsCount; i++)
    printf("  Generic: %s, %s\n", entity.generics[i].name, KWS(entity.generics[i].type));
  for (size_t i=0; i<entity.portsCount; i++)
    printf("  Port: %s, %s\n", entity.ports[i].name, KWS(entity.ports[i].type));
}

/* typedef struct {
     char *name;
     StructGenerics *generics;
     size_t genericsCount;
     StructPorts *ports;
     size_t portsCount;
     StructPorts *bus;
     size_t busCount;
   } StructComponents;
   
   StructComponents *components = NULL;
   int componentsCount = 0;
   
   static void HandleArchitecture(){
     while(GETKEYWORD(0)!= KW_BEGIN){
       TokenNumber++; 
     }
     while(AVOIDOF()){
       /\* if(TokenNumber > 3){
            if(GETKEYWORD(1) == KW_SEMICOLON && GETKEYWORD(2) == KW_RPAREN && GETKEYWORD(0) == KW_END){
          	    TokenNumber--;
          	    break;
          	  }
          	} *\/
       TokenNumber = TokenNumber + 3;
       components = realloc(components, (componentsCount+1)*sizeof(StructComponents));  
       components->name = Tokens[TokenNumber].content;
       printf("%s\n",Tokens[TokenNumber].content);
       TokenNumber++; 
       if (GETKEYWORD(0) == KW_GENERIC) {
         TokenNumber++; 
         printf("Print%s\n",Tokens[TokenNumber].content);
         TokenNumber=+2;
         PAUSE();
     }
   } */

/* Analyzer */
void FileAnalyzer() {
  while(TokenNumber < TokensCount){
    switch (GetKeywordType(Tokens[TokenNumber].content)) {
    case KW_ENTITY:
      HandleEntity();
      break;
    case KW_ARCHITECTURE:
      printf("Found ARCHITECTURE\n");
      /* HandleArchitecture(); */
      break;
    case KW_LIBRARY:
      printf("Found LIBRARY\n");
      break;
    default:
      break;
    }
    TokenNumber++;
  }
}


/*
 * Function: FileReader
 * Opens a file an load in memory its contents.
 */

void FileReader(char *file){
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


static void AddToken(StructToken token){
  if(TokensCount >= TokensCapacity){
    TokensCapacity = TokensCapacity == 0 ? 20 : TokensCapacity + 1;
    Tokens = realloc(Tokens, TokensCapacity * sizeof(StructToken));
  }
  Tokens[TokensCount++] = token;
}


void FileTokenizer(){
  StructToken token;
  
  while(*raw != EOF){
   
    if (*raw == TOKEN_COMMENT){
      HandleComment();
    }
    
    if(*raw == '\n' || *raw == ' ' || *raw == '\t'){
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


