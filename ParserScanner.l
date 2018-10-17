
/*
Author: Ben Klipfel
Created: 11/15/2017
Resources: http://dinosaur.compilertools.net/lex/
*/
Letter	[A-Za-z]
Digit   [0-9]

%{
#include "y.tab.h"
#include "IOMngr.h"
#define YY_INPUT(buf,result,max_size) \
  { int c = GetSourceChar(); \
    result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
  }

bool shouldLog = false;

#define RETURN(val) \
  { if (shouldLog) fprintf(stderr,"Token#=%d, Length=%zu, Text=\"%s\"\n",val,yyleng,yytext); \
    return(val); \
  }

int commentLevel = 0;
int lineComment = 0;
#define IN_COMMENT (lineComment || commentLevel > 0)

%}

%%

  /* Combine comment nesting from Scanner.l with RDScanner.l regular
     expressions except for real literal. Use RETURN(token-symbolic-name)
     where token-symbolic-name appears in yacc file.
   */
"/*"                            { commentLevel++; }
"*/"                            { commentLevel--; }
"//"                            { lineComment = 1;}
int                             { if(!IN_COMMENT) RETURN(INT_TOK);    }
chr                             { if(!IN_COMMENT) RETURN(CHR_TOK);    }
:=                              { if(!IN_COMMENT) RETURN(ASSIGN_TOK); }
\;                              { if(!IN_COMMENT) RETURN(SEMI_TOK);   }
\,                              { if(!IN_COMMENT) RETURN(COMMA_TOK);  }
\(                              { if(!IN_COMMENT) RETURN(LPAREN_TOK); }
\)                              { if(!IN_COMMENT) RETURN(RPAREN_TOK); }
\{                              { if(!IN_COMMENT) RETURN(LBRACE_TOK); }
\}                              { if(!IN_COMMENT) RETURN(RBRACE_TOK); }
\-                              { if(!IN_COMMENT) RETURN(MINUS_TOK);  }
\+                              { if(!IN_COMMENT) RETURN(PLUS_TOK);   }
\*                              { if(!IN_COMMENT) RETURN(TIMES_TOK);  }
\/                              { if(!IN_COMMENT) RETURN(DIV_TOK);    }
{Digit}+                        { if(!IN_COMMENT) RETURN(INTLIT_TOK); }
{Letter}({Letter}|{Digit})*     { if(!IN_COMMENT) RETURN(IDENT_TOK);  }
<<EOF>>                         { RETURN(EOF);                    }
[\n]                            { lineComment = 0; }
[ \t]                           { }
.                               { }
%%
