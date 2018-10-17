%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc/malloc.h>

extern int yylex();	    /* The next token function. */
extern char *yytext;    /* The matched token text.  */
extern int yyleng;      /* The token text length.   */

void yyerror(char *s);

#define YYSTYPE char *   

char * concat(char * a, char * b)
{
  char * ret = malloc(strlen(a) + strlen(b) + 1);
  strcpy(ret,a);
  strcat(ret,b);
  if (malloc_size(a)) free(a); // does not work on linux
  if (malloc_size(b)) free(b);
  return ret;
}

%}

%token IntTok   1   CharTok   2
%token Id       3 	IntLit 		4
%token LParen   5   RParen    6
%token Star     7
%token LBracket 8   RBracket  9
%token Semi     10

%%
Dcls    : Dcls TDcl
Dcls    :
TDcl    : Type Dcl Semi                   { printf("translation: %s %s\n",$2,$1); } ;
Dcl     : PtrSeq DDcl                     { $$ = concat($2,$1); } ;
PtrSeq  : PtrSeq Star                     { $$ = concat($1," pointer to"); } ;
PtrSeq  :                                 { $$ = strdup(""); } ;
DDcl    : Id                              { $$ = concat(yytext,strdup(" is a")); } ;
DDcl    : LParen Dcl RParen               { $$ = $2; } ;
DDcl    : DDcl LParen RParen              { $$ = concat($1," function returning"); } ;
DDcl    : DDcl LBracket Size RBracket     { $$ = concat($1,concat(" array of",$3)); } ;
Size    : IntLit                          { $$ = concat(" ",yytext); } ;
Size    : Id                              { $$ = concat(" ",yytext); } ;
Size    :                                 { $$ = strdup(""); } ;
Type    : IntTok                          { $$ = strdup("int"); } ;
Type    : CharTok                         { $$ = strdup("char"); } ;

%%

int
main(int argc, char *argv[])
{
	yyparse();
}

void
yyerror(char *s)
{
  printf("err: \"%s\" yytext: \"%s\"\n",s,yytext);
}
