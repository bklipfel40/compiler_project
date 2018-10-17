/* Main of Recursive Descent Parser */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "Scanner.h"
#include "IOMngr.h"
#include "RDTokens.h"

#include "RDSupport.h"
#include "RDGrammar.h"

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
    fprintf(stderr,"usage: RecDescent [-l] filename\n");
    return 1;
  }

  char * srcName = malloc(strlen(argv[optind]) + 4 + 1);
  strcpy(srcName,argv[optind]);
  strcat(srcName,".src");
  char * lstName = NULL;
  if (useListing) {
    lstName = malloc(strlen(argv[optind]) + 4 + 1);
    strcpy(lstName,argv[optind]);
    strcat(lstName,".lst");
  }

  if (!OpenFiles(srcName,lstName)) {
    fprintf(stderr,"failed to open %s\n",srcName);
    return 1;
  }

  // to debug token sequencee, set to true
  RDInit(false);
  if (Prog() && CurrentToken() == EOF_TOK) {
    PostMessage(GetCurrentColumn(), 1, "Parse Successful");
  }
  else {
    PostMessage(GetCurrentColumn(), 1, "Parse Unsuccessful");
  }
  CloseFiles();

  return 0;
}
