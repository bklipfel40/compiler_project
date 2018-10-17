/* ScannerDriver.c

*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ScanTokens.h"
#include "SymTab.h"
#include "IOMngr.h"
#include "Scanner.h"

// symbol table entries have two attributes, first line of occurrence and occurrence count
struct Attributes {
  int firstLine;
  int firstColumn;
  int cnt;
};
enum AttrKinds { IGNORE_KIND };

// used with DoFoEntries()
void collectEntries(struct SymEntry * entry, int cnt, void * entryArray) {
  ((struct SymEntry **)entryArray)[cnt] = entry;
}

// used with qsort to sort list of symbol table entries
int entryCmp(const void * A, const void * B) {
  // A is pointer to element of array which contains a pointer to a struct SymEntry
  const char * strA = GetName(*(struct SymEntry **)A);
  const char * strB = GetName(*(struct SymEntry **)B);
  return strcmp(strA,strB);
}

int main(int argc, char **argv) {
  int Token;
  char message[256];
  struct SymTab * table = NULL;

  bool ret = OpenFiles("ScannerSource", "ScannerListing");
  if (!ret) {
    printf("Could not open source and listing files\n");
    exit(1);
  }

  int intSum = 0;
  float floatSum = 0;

  while ((Token = yylex()) != 0) {
    sprintf(message,"Token#=%d, Length=%lu, Text=\"%s\"",Token,yyleng,yytext);
    PostMessage(GetCurrentColumn()-yyleng,yyleng,message);
    switch(Token) {
      case INIT_TOK: {
        // create a symbol table
        if (!table) table = CreateSymTab(20,"main",NULL);
      } break;
      case IDENT_TOK: {
        // place the identifier in the table (if it exists), if new then create and init
        // attributes structure, if already in table then update attributes cnt field, in
        // either case post an appropriate message
        if (!table) {
          PostMessage(1,0,"No SymbolTable\n");
          break;
        }
        bool ret;
        struct SymEntry * entry = LookupName(table,yytext);
        if (entry) {
          struct Attributes * attr = GetAttr(entry);
          attr->cnt++;
          sprintf(message,"occurrence %d",attr->cnt);
          PostMessage(GetCurrentColumn()-yyleng,yyleng,message);
        }
        else {
          struct SymEntry * entry = EnterName(table,yytext);
          struct Attributes * attr = malloc(sizeof(struct Attributes));
          attr->firstLine = GetCurrentLine();
          attr->firstColumn = GetCurrentColumn() - (yyleng+1);
          attr->cnt = 1;
          SetAttr(entry,IGNORE_KIND,attr);
          sprintf(message,"new ident");
          PostMessage(GetCurrentColumn()-yyleng,yyleng,message);
        }
      } break;
      // include cases for int and float tokens
      case INT_TOK: {
        //update accumulated integer and float numbers
        intSum = intSum + atoi(yytext);
      }break;
      case FLOAT_TOK: {
        floatSum = floatSum + atof(yytext);
      }break;
      case EQUAL_TOK: {
        break;
      }
      case DUMP_TOK: {
        printf("intSum = %d\n",intSum);
        printf("floatSum = %f\n",floatSum);
        // get table statistics, alloc an array to hold entry pointers
        struct Stats * stats = Statistics(table);
        struct SymEntry ** entries = malloc(stats->entryCnt * sizeof(struct SymEntry *));

        // enumerate the table collecting entry pointers into the array
        DoForEntries(table,false,collectEntries,0,entries);

        // sort the entries
        qsort(entries,stats->entryCnt,sizeof(struct SymEntry *),entryCmp);

        // list the contents of the table in sorted order
        printf("\nContents of Symbol Table\n");
        printf("                    name  line   col   cnt\n");
        for (int i = 0; i < stats->entryCnt; i++) {
          printf("%3d %20s %5d %5d %5d\n", i,
                 GetName(entries[i]),
                 ((struct Attributes *) GetAttr(entries[i]))->firstLine,
                 ((struct Attributes *) GetAttr(entries[i]))->firstColumn,
                 ((struct Attributes *) GetAttr(entries[i]))->cnt);
        }
        free(stats);
      } break;
    }
  }
  CloseFiles();
}
