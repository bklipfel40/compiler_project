/* IOMngrDriver.c

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "IOMngr.h"

enum States {INIT, IDENT, NUMBR, FINAL};
enum Tokens {EOF_TOK, IDENT_TOK, NUMBR_TOK, EQUAL_TOK, SEMI_TOK, UNKWN_TOK};

int letter = '\0';
int tokenLength;


int LetterKind(int c) {
  if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return 'A';
  if (c >= '0' && c <= '9') return '0';
  if (c == ' ' || c == '\n' || c == '\r') return ' ';
  return c;
}

int GetNextToken() {
  int token = 0;
  int state = 0;
  if (letter == '\0') letter = GetSourceChar();
  tokenLength = 1;
  while (state < FINAL) {
    switch (state) {
      case INIT: {
        switch (LetterKind(letter)) {
          case 'A': {
            state = IDENT; letter = GetSourceChar(); tokenLength++;
          } break;
          case '0': {
            state = NUMBR; letter = GetSourceChar(); tokenLength++;
          } break;
          case '=': {
            state = FINAL; token = EQUAL_TOK; letter = '\0';
          } break;
          case ';': {
            state = FINAL; token = SEMI_TOK; letter = '\0';
          } break;
          case EOF: {
            state = FINAL; token = EOF_TOK; letter = '\0';
          } break;
          case ' ': {
            letter = GetSourceChar();
          } break;
          default: {
            state = FINAL; token = UNKWN_TOK; letter = '\0';
          }
        }
      } break;
      case IDENT: {
        switch (LetterKind(letter)) {
          case 'A': {
            letter = GetSourceChar(); tokenLength++;
          } break;
          default: {
            state = FINAL; token = IDENT_TOK;
          }
        }
      } break;
      case NUMBR: {
        switch (LetterKind(letter)) {
          case '0': {
            letter = GetSourceChar(); tokenLength++;
          } break;
          default: {
            state = FINAL; token = NUMBR_TOK;
          }
        }
      } break;
      default: {
        fprintf(stderr,"unknown state %d\n",state);
      }
    }
  }
  return token;
}

int
main(int argc, char **argv)
{ char *src, *lst;

  // collect command line arguments for source and listing files
	if (argc == 1) {
	  src = "IOMngrSource";
		lst = "IOMngrListing";
	}
  else if (argc == 2) {
    src = argv[1];
    lst = NULL;
  }
  else if (argc == 3) {
    src = argv[1];
    lst = argv[2];
  }
  else {
    fprintf(stderr,"usage: IOMngrDriver [SourceName [ListingName]]\n");
    exit(1);
  }

  if (OpenFiles(src,lst)) {
    // we delibertly bump up against the EOF three times
    int eofCnt = 0;
    while(eofCnt < 3) {
      int token = GetNextToken();
      switch (token) {
        case EOF_TOK: {
          eofCnt++;
          PostMessage(GetCurrentColumn(),1,"EOF found");
        } break;
        case IDENT_TOK: {
          tokenLength--;
          PostMessage(GetCurrentColumn()-tokenLength,tokenLength, "IDENT found");
        } break;
        case NUMBR_TOK: {
          tokenLength--;
          PostMessage(GetCurrentColumn()-tokenLength,tokenLength, "NUMBR found");
        } break;
        case EQUAL_TOK: {
          PostMessage(GetCurrentColumn(),1, "EQUAL found");
        } break;
        case SEMI_TOK: {
          PostMessage(GetCurrentColumn(),1, "SEMI found");
        } break;
        case UNKWN_TOK: {
          PostMessage(GetCurrentColumn(),1, "UNKWN found");
        } break;
      }
   }
    CloseFiles();
  }
  else {
    printf("Files could not be opened.\n");
  }

  exit(0);
}
