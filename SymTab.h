/* SymTab.h

  This file defines the interface to a simple symbol table.
  For the purposes of this module a symbol table is abstractly
  viewed as a collection of entries each associating a name
  with a single attribute which is taken to be a (void *).
  This allows the user to associate a name with an arbitrary
  structure of attribute information which the user creates
  and maintains. A symbol table can optionally reference a
  parent symbol table to support names in nested scopes.

  The following functionality is provided
  - symbol table creation and destruction
  - getting and setting an entry's attribute pointers
  - getting an entry's name string
  - enumerating the contents of the table
*/

#ifndef SYMTAB_H
#define SYMTAB_H

#include <stdbool.h>

/* The Name/Attributes association structure used in the symbol tables
   linked list. The attrKind field allows the user to distinguish between
   different kinds of attribute data if necessary.
*/
struct SymEntry {
  struct SymTab * table;
  char *name;
  int attrKind;
  void *attributes;
  struct SymEntry *next;
};

/* The symbol table structure proper. The hash table array Contents
   is dynamically allocated according to size
*/
struct SymTab {
  struct SymTab * parent;
  char *scopeName;
  int size;
  struct SymEntry **contents;
};

/* CreateSymTab   Create and return a reference to a symbol table of
                  approximately Size many entries. ScopeName is a name to
                  describe the scope of the table and maybe NULL. If
                  parentTable is not NULL, the new table will reference it.
                  Returns a pointer to the table or NULL if unable to create
                  the table.

   DestroySymTab  Destroy all storage associated with a Symbol Table which
                  is under the table's control. This function can only free
                  storage that has been allocated by this module. Any
                  storage allocated for attributes associated with an entry
                  must be freed before the table is destroyed. This can be
                  done by creating an entryFree() function and using the
                  DoForEntries() function described below. Returns parent
                  table if it exists.
*/
struct SymTab *   CreateSymTab(int size, char * scopeName, struct SymTab * parentTable);
struct SymTab *   DestroySymTab(struct SymTab *aTable);
void              entryFree( struct SymEntry * anEntry );

/* LookupName     Search aTable, and if necessary parent tables, for an
                  entry for aName. Returns the first entry found or NULL.

   EnterName      Create an new entry for aName in aTable if it does not
                  exist. Return a reference to the entry.
*/
struct SymEntry * LookupName(struct SymTab *aTable, const char * aName);
struct SymEntry * EnterName(struct SymTab *aTable, const char * aName);

/* GetAttrKind    Return the "kind" (e.g. user determined integer) of the attribute.
   SetAttr        Set the attribute pointer associated with an entry.
   GetAttr        Return the attribute pointer associated with an entry.
   GetName        Return the name string associated with an entry.
   GetTable       Return the table associated with the entry.
   GetScopeName   Return the scope name of the table (specified at creation)
   GetScope       Return a new string combining the scope name of aTable
                  and all parent tables. The caller is responsible for
                  freeing the returned string.
   GetParentTable Return the parent table or NULL.
*/
int               GetAttrKind(struct SymEntry *anEntry);
void              SetAttr(struct SymEntry *anEntry, int kind, void *attributes);
void *            GetAttr(struct SymEntry *anEntry);
const char *      GetName(struct SymEntry *anEntry);
struct SymTab *   GetTable(struct SymEntry *anEntry);
const char *      GetScopeName(struct SymTab *aTable);
char *            GetScope(struct SymTab *aTable);
char *            DblString( char* first, char* carrot, char* second);
struct SymTab *   GetParentTable(struct SymTab *aTable);

/* DoForEntries   Call entryFunc() for each entry contained in aTable. The function
                  will be provided with a pointer to an entry, a sequential count
                  for the entry and a pointer to void for any additional required
                  arguments. If includeParentTable is true recursively call this
                  function for the parent table when all entries in aTable have
                  been processed. StartCnt is the count to begin the enumeration
                  with. WithArgs is additional data that will be supplied to
                  entryFunc(). The implementation will invoke the function with
                  "entryFunc(someEntry,cnt,withArgs)".
*/
void              DoForEntries(struct SymTab *aTable, bool includeParentTable,
                    void (*entryFunc)(struct SymEntry * entry, int cnt, void * args),
                    int startCnt, void * withArgs);

/* Statistics     Return a structure containing table statistics.
                  minLen is the length of the shortest chain in the hash table
                  maxLen is the length of the longest chain in the hash table
                  avgLen is the average chain length
                  entryCnt is the sum of the chain lengths
*/
struct Stats { int minLen;
               int maxLen;
               int avgLen;
               int entryCnt;
};
struct Stats *     Statistics(struct SymTab *aTable);

#endif
