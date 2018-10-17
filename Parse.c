/* Main of Parser */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Scanner.h"
#include "IOMngr.h"
#include "Grammar.h"

int
main(int argc, char * argv[])
{
  if (argc != 2 || strlen(argv[1]) == 0) {
    fprintf(stderr,"usage: Parse filename\n");
    return 1;
  }

  char * srcName = malloc(strlen(argv[1]) + 4 + 1);
  strcpy(srcName,argv[1]);
  strcat(srcName,".src");
  char * lstName = malloc(strlen(argv[1]) + 4 + 1);
  strcpy(lstName,argv[1]);
  strcat(lstName,".lst");

  if (!OpenFiles(srcName,lstName)) {
    printf("Source File Not Found.\n");
    return 1;
  }

  if (yyparse() == 0) {
    PostMessage(0,1,"Parse Successful");
  }
  else {
    PostMessage(0,1,"Parse Unsuccessful");
  }

  CloseFiles();

  return 0;
}
