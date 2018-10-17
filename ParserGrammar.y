%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "IOMngr.h"

extern int yylex();    /* The next token function. */
extern char *yytext;   /* The matched token text.  */
extern int yyleng;     /* The token text length.   */

void yyerror(char *s);

#define YYSTYPE long   /* 64 bit so can hold pointer and int */

%}

%token INT_TOK     1  CHR_TOK    2
%token ASSIGN_TOK  3  SEMI_TOK   4  COMMA_TOK    5
%token LPAREN_TOK  6  RPAREN_TOK 7
%token LBRACE_TOK  8  RBRACE_TOK 9
%token MINUS_TOK  10  PLUS_TOK  11  TIMES_TOK   12   DIV_TOK    13
%token INTLIT_TOK 14  IDENT_TOK 15

%%
Prog    : IDENT_TOK LBRACE_TOK StmtSeq RBRACE_TOK  {};
StmtSeq : Stmt SEMI_TOK StmtSeq                    {};
StmtSeq :                                          {};
Stmt    : Decl                                     {};
Stmt    : Assign                                   {};
Decl    : Type IDLst                               {};
Type    : INT_TOK                                  { $$ = (long) strdup(yytext); };
Type    : CHR_TOK                                  { $$ = (long) strdup(yytext); };
IDLst   : IDENT_TOK MLst                           {};
MLst    : COMMA_TOK IDLst                          {};
MLst    :                                          {};
Assign  : LHS ASSIGN_TOK Expr                      { printf("%s =\n",(char *)$1); };
Expr    : Term MExpr                               {};
MExpr   : AddOp Term MExpr                         { printf("%s ", $1);     };
MExpr   :                                          {};
Term    : Factor MTerm                             {};
MTerm   : MultOp Factor MTerm                      { printf("%s ", $1); };
MTerm   :                                          {};
Factor  : LPAREN_TOK Expr RPAREN_TOK               {};
Factor  : MINUS_TOK Factor                         {};
Factor  : INTLIT_TOK                               { printf("%s ", yytext); };
Factor  : IDENT_TOK                                { printf("%s ", yytext); };
AddOp   : MINUS_TOK                                { $$ = (long) strdup(yytext); };
AddOp   : PLUS_TOK                                 { $$ = (long) strdup(yytext); };
MultOp  : TIMES_TOK                                { $$ = (long) strdup(yytext); };
MultOp  : DIV_TOK                                  { $$ = (long) strdup(yytext); };
LHS     : IDENT_TOK                                { $$ = (long) strdup(yytext); };
%%

void
yyerror(char *s)
{
  char msg[256];
  snprintf(msg,255,"err: \"%s\" yytext: \"%s\"\n",s,yytext);
  PostMessage(GetCurrentColumn(),yyleng,msg);
}
