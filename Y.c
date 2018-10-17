/* Main of Y Language Compiler */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>
#include <unistd.h>

#include "Scanner.h"
#include "IOMngr.h"
#include "Grammar.h"
#include "YSemantics.h"

int
main(int argc, char * argv[])
{
  int useListing = 0;
  int c;
  opterr = 0;
  while ((c = getopt(argc,argv,"l")) != -1) {
    switch (c) {
      case 'l':
        useListing = 1;
        break;
      case '?':
        if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
        return 1;
      default:
        break;
    }
  }

  if (optind >= argc) {
    fprintf(stderr,"usage: Y [-l] base-filename\n");
    return 1;
  }

  char * srcName = malloc(strlen(argv[optind]) + 4 + 1);
  strcpy(srcName,argv[optind]);
  strcat(srcName,".src");

  char * asmName = malloc(strlen(argv[optind]) + 4 + 1);
  strcpy(asmName,argv[optind]);
  strcat(asmName,".asm");

  char * lstName = NULL;
  if (useListing) {
    lstName = malloc(strlen(argv[optind]) + 4 + 1);
    strcpy(lstName,argv[optind]);
    strcat(lstName,".lst");
  }

  if (!OpenFiles(srcName,lstName)) {
    printf("Source File Not Found.\n");
    exit(1);
  }

  InitSemantics();
  InitCodeGen(asmName);

  if (yyparse() == 0) {
    PostMessage(0,1,"Parse Successful");
  }
  else {
    PostMessage(0,1,"Parse Unsuccessful");
  }

  CloseFiles();

  exit(0);
}
