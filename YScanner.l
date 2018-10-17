Letter	[A-Za-z]
Digit   [0-9]
Symbol  [\`\~\!\@\#\$\%\^\&\*\(\)\-\_\=\+\[\]\{\}\;\:\'\"\,\.\<\>\/\?]

%{
#include "IOMngr.h"
#define YY_INPUT(buf,result,max_size) \
  { int c = GetSourceChar(); \
		result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
  }

bool shouldLog = false;
int commentLevel = 0;
int lineComment = 0;

#define RETURN(val) \
  { if (shouldLog) fprintf(stderr,"Token#=%d, Length=%zu, Text=\"%s\"\n",val,yyleng,yytext); \
    return(val); \
  }

#define IN_COMMENT (lineComment || commentLevel > 0)

#include "YSemantics.h"
#include "y.tab.h"
%}

%%
"/*"                            { commentLevel++; }
"*/"                            { commentLevel--; }
impl                            { if (!IN_COMMENT) RETURN(IMPL_TOK);  }
decl                            { if (!IN_COMMENT) RETURN(DECL_TOK);  }
int                             { if (!IN_COMMENT) RETURN(INT_TOK);   }
chr                             { if (!IN_COMMENT) RETURN(CHR_TOK);   }
put                             { if (!IN_COMMENT) RETURN(PUT_TOK);   }
get                             { if (!IN_COMMENT) RETURN(GET_TOK);   }
if                              { if (!IN_COMMENT) RETURN(IF_TOK);    }
else                            { if (!IN_COMMENT) RETURN(ELSE_TOK);  }
while                           { if (!IN_COMMENT) RETURN(WHILE_TOK); }
\{                              { if (!IN_COMMENT) RETURN('{'); }
\}                              { if (!IN_COMMENT) RETURN('}'); }
"("                             { if (!IN_COMMENT) RETURN('('); }
")"                             { if (!IN_COMMENT) RETURN(')'); }
":"                             { if (!IN_COMMENT) RETURN(':'); }
";"                             { if (!IN_COMMENT) RETURN(';'); }
","                             { if (!IN_COMMENT) RETURN(','); }
"*"                             { if (!IN_COMMENT) RETURN('*'); }
"/"                             { if (!IN_COMMENT) RETURN('/'); }
"+"                             { if (!IN_COMMENT) RETURN('+'); }
"-"                             { if (!IN_COMMENT) RETURN('-'); }
"="                             { if (!IN_COMMENT) RETURN('='); }
\'({Letter}|{Digit}|{Symbol}|\\n|\\t|\ )\' { if (!IN_COMMENT) RETURN(CHRLIT_TOK);}
{Letter}({Letter}|{Digit})*			           { if (!IN_COMMENT) RETURN(IDENT_TOK); }
{Digit}{Digit}*                            { if (!IN_COMMENT) RETURN(INTLIT_TOK);}
\"({Letter}|{Digit}|{Symbol}|\\n|\\t|\ )*\" { if(!IN_COMMENT) RETURN(STRLIT_TOK);}
">"                             { if (!IN_COMMENT) RETURN(GT_TOK); }
">="                            { if (!IN_COMMENT) RETURN(GTE_TOK);}
"<"                             { if (!IN_COMMENT) RETURN(LT_TOK); }
"<="                            { if (!IN_COMMENT) RETURN(LTE_TOK);}
"=="                            { if (!IN_COMMENT) RETURN(EQ_TOK); }
"##"                            { if (!IN_COMMENT) RETURN(NEQ_TOK);}
[ \t]                           { }
"//".*                          { lineComment = 1; }
[\n]                            { lineComment = 0; }
.                               { }
%%
