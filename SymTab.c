#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#include "SymTab.h"

/*
 * Author: Ben Klipfel
 * Created: 9-12-17
 * Resources: https://www.tutorialspoint.com/c_standard_library/string_h.htm
 *            https://www.tutorialspoint.com/cprogramming/c_pointer_to_pointer.htm
              http://joequery.me/code/snprintf-c/
              Kala Arentz helped me with calloc
 */
//About 198 Lines of Code minus my comments
struct SymTab *
CreateSymTab(int size, char * scopeName, struct SymTab * parentTable) {
    //calloc space for a SymTab
    struct SymTab * newTable = calloc(1, sizeof(struct SymTab));
    //check and see if scopeName is NULL
    if( scopeName == NULL ){
        scopeName = "";
    }
    newTable->scopeName = strdup(scopeName);
    newTable->size = size;
    //instantiate all entries to NULL
    newTable->contents = calloc(size,sizeof(struct SymEntry*));
    newTable->parent = parentTable;
    return newTable;
}

//this will make destroy sym tab a lot cleaner
void
entryFree(struct SymEntry * anEntry) {
  //free the name
  free(anEntry->name);
  //free the entry
  free(anEntry);
}

struct SymTab *
DestroySymTab(struct SymTab *aTable) {
  struct SymTab * parent = aTable->parent;
  //free every entry
  DoForEntries(aTable, false, entryFree, 0, NULL);
  //free the contents and table itself
  free(aTable->contents);
  free(aTable);
  return parent;
}

int
HashName(int size, const char *name) {
    int val = 0;
    int count = strlen(name);
    //sum up the values of the characters in name
    for( int i = 0; i < count; i++){
        val += name[i];
    }
    //mod that by the size of the table
    val = val % size;
    //return that value
    return val;
}

struct SymEntry *
FindHashedName(struct SymTab *aTable, int hashValue, const char *name) {
  if(aTable == NULL){
    return NULL;
  }
    //create a reference to search for the hashedName
    struct SymEntry *search = aTable->contents[hashValue];
    while( search ){
        if(strcmp(search->name,name)==0){
            return search;
        }
        search = search->next;
    }
    return NULL;
}

struct SymEntry *
LookupName(struct SymTab *aTable, const char * name) {
  //get hash of name for aTable
    int hash = HashName(aTable->size, name);
    //see if we can find the hashedName
    struct SymEntry * sym = FindHashedName(aTable, hash, name);
    if( sym ){
      return sym;
    }
    else{
      if( aTable->parent){
        //check parent table RECURSION
        return LookupName(aTable->parent, name);
      }
      else{
        return NULL;
      }
    }
}

struct SymEntry *
EnterName(struct SymTab *aTable, const char *name) {
    //lookup the name and see if it exists (use existing method)
    int hashedName = HashName( aTable->size, name);
    struct SymEntry * check = LookupName(aTable, name);
    if( check && strcmp(check->table->scopeName, aTable->scopeName)==0){
        return check;
    }
    else{
      struct SymEntry *new = malloc(sizeof(struct SymEntry));
      new->name = strdup(name);
      new->table = aTable;
      new->attributes = NULL;
      new->next = aTable->contents[hashedName];
      aTable->contents[hashedName] = new;
      return new;
    }
}

int
GetAttrKind(struct SymEntry *anEntry) {
    return anEntry->attrKind;
}

void
SetAttr(struct SymEntry *anEntry, int kind, void *attributes) {
    //set the attrKind of anEntry
    anEntry->attrKind = kind;
    //set the attributes of SymEntry
    anEntry->attributes = attributes;
}

void *
GetAttr(struct SymEntry *anEntry) {
    if( anEntry ){
        if(!anEntry->attributes){
          return NULL;
        }
        return anEntry->attributes;
    }
    return NULL;
}

const char *
GetName(struct SymEntry *anEntry) {
  if( anEntry == NULL ){
    return NULL;
  }
    return anEntry->name;
}

struct SymTab *
GetTable(struct SymEntry *anEntry) {
  if( anEntry == NULL ){
    return NULL;
  }
    return anEntry->table;
}

const char *
GetScopeName(struct SymTab *aTable) {
  if( aTable == NULL ){
    return NULL;
  }
    //literal name of scope
    return aTable->scopeName;
}

char *
GetScope(struct SymTab *aTable) {
  struct SymTab * reference = aTable->parent;
  // strdup(GetScopeName(aTable))
  char * curScope = strdup(GetScopeName(aTable));
  // if no parent return
  while( reference ){
    curScope = DblString(reference->scopeName,">",curScope);
    reference = reference->parent;
  }
  //if no parent return
  return curScope;
}

//Helper method to combine two strings
//carrot is the >
char *
DblString( char *first, char *carrot, char *second ){
  //make room for null and >
  int totalLen = strlen(first) + strlen(second) +2;
  //use both as a buffer
  char * bothStrings = malloc( sizeof(char) * totalLen);
  //use snprintf
  snprintf(bothStrings, totalLen, "%s%s%s", first, carrot, second);
  return bothStrings;
}

struct SymTab *
GetParentTable(struct SymTab *aTable) {
  if( aTable ){
    if(aTable->parent){
      return aTable->parent;
    }
    return NULL;
  }
    return NULL;
}

void
DoForEntries(struct SymTab *aTable, bool includeParentTable,
             void (*entryFunc)(struct SymEntry * entry, int cnt, void * args),
             int startCnt, void * withArgs) {
    //loop over contents
    for( int i = 0; i < aTable->size; i++ ){
        struct SymEntry * walk = aTable->contents[i];
        while( walk ){//walk the list
            //call entryFunc for each entry
            entryFunc( walk, startCnt, withArgs);
            walk = walk->next;
            startCnt++;
        }
    }
    //recursivley call if includeParentTable is true
    if( includeParentTable ){
      if( aTable->parent ){
        DoForEntries( aTable->parent, true, entryFunc, startCnt, withArgs);
      }
    }
}

struct Stats *
Statistics(struct SymTab *aTable) {
    if( aTable == NULL){
        return NULL;
    }
    struct Stats* stat = calloc(1,sizeof(struct Stats));
    //initialize the avgLen and entryCnt
    stat->avgLen = 0;
    stat->entryCnt = 0;
    int lengths[aTable->size];
    for( int i = 0; i< aTable->size; i++){
        lengths[i] = 0;
        struct SymEntry * head = aTable->contents[i];
        //walk each head of the symEntries
        while(head){
            stat->entryCnt = stat->entryCnt +1;
            lengths[i] = lengths[i]+1;
            head = head->next;
        }
    }
    stat->avgLen = stat->entryCnt/(aTable->size);
    //initialize the min and max lengths
    //stat->minLen = lengths[0];
    stat->minLen = lengths[0];
    stat->maxLen = lengths[0];
    //adjust the lengths (linear time for now)
    for( int i = 0; i < aTable->size; i++){
        if( lengths[i] < stat->minLen){
            stat->minLen = lengths[i];
        }
        else if( lengths[i] > stat->maxLen ){
            stat->maxLen = lengths[i];
        }
    }
    return stat;
  }
