Letter [A-Za-z]
Digit [0-9]

%%
int         {  return 1; }
char        {  return 2; }
{Letter}+ 	{  return 3; }
{Digit}+ 		{  return 4; }
\(       		{  return 5; }
\)       		{  return 6; }
\*       		{  return 7; }
\[       		{  return 8; }
\]       		{  return 9; }
\;       		{  return 10; }

[ \n\t]		{ }
.					{ }
%%

