/* Semantics.h
 The action and supporting routines for performing semantics processing.
 */

#include "SymTab.h"
#include "IOMngr.h"
#include "Scanner.h"
#include "YCodeGen.h"

// Declaration of semantic record data types used in grammar.y %union
struct IdList {
  struct SymEntry * entry;
  struct IdList * next;
};

enum BaseTypes { IntBaseType, ChrBaseType, StrBaseType };

struct FuncDesc {
  enum BaseTypes returnType;
  struct InstrSeq * funcCode;
};

//create a string literal
struct StringLiteral{
  struct InstrSeq * instrSeq;
  char * name;
};

enum DeclTypes { PrimType, FuncType };

struct TypeDesc {
  enum DeclTypes declType;
  union {
    enum BaseTypes primDesc;
    struct FuncDesc * funcDesc;
  };
};

//Create a struct ExprResult which holds both the code sequence that computes the
//expression and the register number where the result is located when the code executes.
//This will get added as an option for the "union" and associated with certain nonterminals.
//You may also want an enumerated type for the various operators or simply use the operator character
struct ExprResult {
  struct InstrSeq * instrSeq;
  int reg;
  enum BaseTypes type;
};

//need a CondResult struct similiar to ExprResult, remembers the code sequence and the
//label used in the conditional branch instruction
struct CondResult{
  struct InstrSeq * instrSeq;
  char * label;
};

enum Operators {AddOp, SubOp, MultOp, DivOp};
enum CompareOperators {GT,GTE,LT,LTE,EQ,NEQ};

// Symbol Table Structures
struct Attr {
  struct TypeDesc * typeDesc;
  char * reference;
};

struct StringAttr{
  char * label;
  char * value;
};

// Supporting Routines

void InitSemantics();
void ListIdentifierTable();

struct TypeDesc * MakePrimDesc(enum BaseTypes type);
struct TypeDesc * MakeFuncDesc(enum BaseTypes returnType);

// Semantics Actions
void                    Finish();

void                    ProcDecls(struct IdList * idList, enum BaseTypes baseType);
struct IdList *         AppendIdList(struct IdList * item, struct IdList * list);
struct IdList *         ProcName(char * id, enum DeclTypes type);
void                    ProcFunc(char * id, struct InstrSeq * instrs);

//InstrSeq
//semantics 2
struct InstrSeq *       ProcAssign( char * id, struct ExprResult * expr);
struct InstrSeq *       ProcPut(struct ExprResult * chars);
//semantics 3
struct InstrSeq *       ProcIfFunc(struct CondResult * condResult, struct InstrSeq * instrSeq);
struct InstrSeq *       ProcIfElseFunc(struct CondResult * condResult, struct InstrSeq * ifInstrSeq, struct InstrSeq * elseInstrSeq);
struct InstrSeq *       ProcWhileLoop(struct CondResult * condResult, struct InstrSeq * instrSeq);



//ExprResults
//semantics 2
struct ExprResult *     ProcAdd( struct ExprResult * r1, enum Operators operators, struct ExprResult * r2);
struct ExprResult *     ProcMult(struct ExprResult * r1, enum Operators operators, struct ExprResult * r2);
struct ExprResult *     ProcNegate(struct ExprResult * expr);
struct ExprResult *     ProcIntLit(char * intlit);
struct ExprResult *     ProcChrLit(char * chars);
struct ExprResult *     ProcId(char * id);
struct ExprResult *     ProcGet( int getType );
//semantics 3
struct CondResult *     ProcCondOp(struct ExprResult *r1, enum CompareOperators cond, struct ExprResult *r2);
//semantics 4
char *                  ProcStrLit(char * string);
struct InstrSeq *       ProcPutStrLit(char * string);
