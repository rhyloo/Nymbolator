#include <sys/stat.h>

#include <ctype.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TOK_IDENT	'I'
#define TOK_NUMBER	'N'
#define TOK_CONST	'C'
#define TOK_VAR		'V'
#define TOK_PROCEDURE	'P'
#define TOK_CALL	'c'
#define TOK_BEGIN	'B'
#define TOK_END		'E'
#define TOK_IF		'i'
#define TOK_THEN	'T'
#define TOK_WHILE	'W'
#define TOK_DO		'D'
#define TOK_ODD		'O'
#define TOK_DOT		'.'
#define TOK_EQUAL	'='
#define TOK_COMMA	','
#define TOK_SEMICOLON	';'
#define TOK_ASSIGN	':'
#define TOK_HASH	'#'
#define TOK_LESSTHAN	'<'
#define TOK_GREATERTHAN	'>'
#define TOK_PLUS	'+'
#define TOK_MINUS	'-'
#define TOK_MULTIPLY	'*'
#define TOK_DIVIDE	'/'
#define TOK_LPAREN	'('
#define TOK_RPAREN	')'

/*
 * pl0c -- PL/0 compiler.
 *
 * program	= block "." .
 * block	= [ "const" ident "=" number { "," ident "=" number } ";" ]
 *		  [ "var" ident { "," ident } ";" ]
 *		  { "procedure" ident ";" block ";" } statement .
 * statement	= [ ident ":=" expression
 *		  | "call" ident
 *		  | "begin" statement { ";" statement } "end"
 *		  | "if" condition "then" statement
 *		  | "while" condition "do" statement ] .
 * condition	= "odd" expression
 *		| expression ( "=" | "#" | "<" | ">" ) expression .
 * expression	= [ "+" | "-" ] term { ( "+" | "-" ) term } .
 * term		= factor { ( "*" | "/" ) factor } .
 * factor	= ident
 *		| number
 *		| "(" expression ")" .
 */

static char *raw, *token;
static int depth, type;
static size_t line = 1;

static void expression(void);

/*
 * Misc. functions.
 */

static void
error(const char *fmt, ...)
{
	va_list ap;

	(void) fprintf(stderr, "pl0c: error: %lu: ", line);

	va_start(ap, fmt);
	(void) vfprintf(stderr, fmt, ap);
	va_end(ap);

	(void) fputc('\n', stderr);

	exit(1);
}

static void
readin(char *file)
{
	int fd;
	struct stat st;

	if (strrchr(file, '.') == NULL)
		error("file must end in '.pl0'");

	if (!!strcmp(strrchr(file, '.'), ".pl0"))
		error("file must end in '.pl0'");

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

/*
 * Lexer.
 */

static void
comment(void)
{
	int ch;

	while ((ch = *raw++) != '}') {
		if (ch == '\0')
			error("unterminated comment");

		if (ch == '\n')
			++line;
	}
}

static int
ident(void)
{
	char *p;
	size_t i, len;

	p = raw;
	while (isalnum(*raw) || *raw == '_')
		++raw;

	len = raw - p;

	--raw;

	free(token);

	if ((token = malloc(len + 1)) == NULL)
		error("malloc failed");

	for (i = 0; i < len; i++)
		token[i] = *p++;
	token[i] = '\0';

	if (!strcmp(token, "const"))
		return TOK_CONST;
	else if (!strcmp(token, "var"))
		return TOK_VAR;
	else if (!strcmp(token, "procedure"))
		return TOK_PROCEDURE;
	else if (!strcmp(token, "call"))
		return TOK_CALL;
	else if (!strcmp(token, "begin"))
		return TOK_BEGIN;
	else if (!strcmp(token, "end"))
		return TOK_END;
	else if (!strcmp(token, "if"))
		return TOK_IF;
	else if (!strcmp(token, "then"))
		return TOK_THEN;
	else if (!strcmp(token, "while"))
		return TOK_WHILE;
	else if (!strcmp(token, "do"))
		return TOK_DO;
	else if (!strcmp(token, "odd"))
		return TOK_ODD;

	return TOK_IDENT;
}

static int
number(void)
{
	const char *errstr;
	char *p;
	size_t i, j = 0, len;

	p = raw;
	while (isdigit(*raw) || *raw == '_')
		++raw;

	len = raw - p;

	--raw;

	free(token);

	if ((token = malloc(len + 1)) == NULL)
		error("malloc failed");

	for (i = 0; i < len; i++) {
		if (isdigit(*p))
			token[j++] = *p;
		++p;
	}
	token[j] = '\0';

	(void) strtonum(token, 0, LONG_MAX, &errstr);
	if (errstr != NULL)
		error("invalid number: %s", token);

	return TOK_NUMBER;
}

static int
lex(void)
{

again:
	/* Skip whitespace.  */
	while (*raw == ' ' || *raw == '\t' || *raw == '\n') {
		if (*raw++ == '\n')
			++line;
	}

	if (isalpha(*raw) || *raw == '_')
		return ident();

	if (isdigit(*raw))
		return number();

	switch (*raw) {
	case '{':
		comment();
		goto again;
	case '.':
	case '=':
	case ',':
	case ';':
	case '#':
	case '<':
	case '>':
	case '+':
	case '-':
	case '*':
	case '/':
	case '(':
	case ')':
		return (*raw);
	case ':':
		if (*++raw != '=')
			error("unknown token: ':%c'", *raw);

		return TOK_ASSIGN;
	case '\0':
		return 0;
	default:
		error("unknown token: '%c'", *raw);
	}

	return 0;
}

/*
 * Parser.
 */

static void next(void)
{

	type = lex();
	++raw;
}

static void expect(int match)
{

	if (match != type)
		error("syntax error");

	next();
}

static void
factor(void)
{

	switch (type) {
	case TOK_IDENT:
	case TOK_NUMBER:
		next();
		break;
	case TOK_LPAREN:
		expect(TOK_LPAREN);
		expression();
		expect(TOK_RPAREN);
	}
}

static void
term(void)
{

	factor();

	while (type == TOK_MULTIPLY || type == TOK_DIVIDE) {
		next();
		factor();
	}
}

static void
expression(void)
{

	if (type == TOK_PLUS || type == TOK_MINUS)
		next();

	term();

	while (type == TOK_PLUS || type == TOK_MINUS) {
		next();
		term();
	}
}

static void
condition(void)
{

	if (type == TOK_ODD) {
		expect(TOK_ODD);
		expression();
	} else {
		expression();

		switch (type) {
		case TOK_EQUAL:
		case TOK_HASH:
		case TOK_LESSTHAN:
		case TOK_GREATERTHAN:
			next();
			break;
		default:
			error("invalid conditional");
		}

		expression();
	}
}

static void statement(void){

	switch (type) {
	case TOK_IDENT:
		expect(TOK_IDENT);
		expect(TOK_ASSIGN);
		expression();
		break;
	case TOK_CALL:
		expect(TOK_CALL);
		expect(TOK_IDENT);
		break;
	case TOK_BEGIN:
		expect(TOK_BEGIN);
		statement();
		while (type == TOK_SEMICOLON) {
			expect(TOK_SEMICOLON);
			statement();
		}
		expect(TOK_END);
		break;
	case TOK_IF:
		expect(TOK_IF);
		condition();
		expect(TOK_THEN);
		statement();
		break;
	case TOK_WHILE:
		expect(TOK_WHILE);
		condition();
		expect(TOK_DO);
		statement();
	}
}

static void block(void)
{

	if (depth++ > 1)
		error("nesting depth exceeded");

	if (type == TOK_CONST) {
		expect(TOK_CONST);
		expect(TOK_IDENT);
		expect(TOK_EQUAL);
		expect(TOK_NUMBER);
		while (type == TOK_COMMA) {
			expect(TOK_COMMA);
			expect(TOK_IDENT);
			expect(TOK_EQUAL);
			expect(TOK_NUMBER);
		}
		expect(TOK_SEMICOLON);
	}

	if (type == TOK_VAR) {
		expect(TOK_VAR);
		expect(TOK_IDENT);
		while (type == TOK_COMMA) {
			expect(TOK_COMMA);
			expect(TOK_IDENT);
		}
		expect(TOK_SEMICOLON);
	}

	while (type == TOK_PROCEDURE) {
		expect(TOK_PROCEDURE);
		expect(TOK_IDENT);
		expect(TOK_SEMICOLON);

		block();

		expect(TOK_SEMICOLON);
	}

	statement();

	if (--depth < 0)
		error("nesting depth fell below 0");
}

static void parse(void){

	next();
	block();
	expect(TOK_DOT);

	if (type != 0)
		error("extra tokens at end of file");
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


