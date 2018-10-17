
Letter	[A-Za-z]
Digit		[0-9]
WhiteSpace [ \t\n]

%%

BEGIN                   { printf("// Reserved word \"BEGIN\"//\n"); }
{Digit}+                { printf("Number\n"); }
{Letter}{Letter}*       { printf("Identifier: len: %zu text:\"%s\"\n",yyleng,yytext); }
\[{Digit}(,{Digit})*\]  { printf("Seq of digits\n"); }
{WhiteSpace}            { }
.                       { }

%%
