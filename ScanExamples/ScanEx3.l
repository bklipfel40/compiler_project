
Letter	[A-Za-z]
Digit		[0-9]

%%

BEGIN                   { return 1; }
{Digit}+                { return 2; }
{Letter}{Letter}*       { return 3; }
\[{Digit}(,{Digit})*\]  { return 4; }
[ \t\n]                 { }
.                       { }

%%


int
main(int argc, char * argv[]) {
  int token = 0;
  while ((token = yylex()) != 0) {
    printf("token %d \"%s\"\n",token,yytext);
  }
}
