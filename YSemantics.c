/* Semantics.c
 Support and semantic action routines.

 */
/*
 * Author: Ben Klipfel
 * Date: 12/3/2017
 * Resources: Kala Arentz, Anthony Smith, Suzy Ratcliffe
 * Resources: https://stackoverflow.com/questions/19827522/difference-between-move-and-li-in-mips-assembly-language
 * Resources: http://www.cs.uwm.edu/classes/cs315/Bacon/Lecture/HTML/ch05s03.html
 */
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "YSemantics.h"

// Shared Data

struct SymTab * IdentifierTable;
struct SymTab * StringLiteralTable;
struct InstrSeq * SharedSequence;
enum AttrKinds { VOID_KIND = -1, INT_KIND, STRING_KIND, STRUCT_KIND };

char * BaseTypeNames[2] = { "int", "chr"};
char * TypeNames[2] = { "", "func"};

// corresponds to enum Operators
char * Ops[] = { "add", "sub", "mul", "div"};
char * BoolOps[] = { "and", "or", "not" };

// corresponds to negation of enum Comparisons
// enum Comparisons { LtCmp, LteCmp, GtCmp, GteCmp, EqCmp, NeqCmp };
char * Branches[] = { "bge", "bg", "ble", "bl", "bne", "beq"};
// comparison set instructions, in order with enum Comparisons
char * CmpSetReg[] = { "slt", "sle", "sgt", "sge", "seq", "sne" };


// Supporting Routines
void
PostMessageAndExit(int col, char * message) {
  PostMessage(col,1,message);
  CloseFiles();
  exit(0);
}

void
InitSemantics() {
  IdentifierTable = CreateSymTab(100,"global",NULL);
  //create the string literal Table
  StringLiteralTable = CreateSymTab(100,"global",NULL);
  //create the shared instruction sequence code to be used globally
  SharedSequence = GenInstr(NULL,".data",NULL,NULL,NULL);
}

char *
StringForType(struct TypeDesc * desc) {
  switch (desc->declType) {
    case PrimType: {
      return strdup(BaseTypeNames[desc->primDesc]);
    } break;
    case FuncType: {
      return strdup(BaseTypeNames[desc->funcDesc->returnType]);
    } break;
  }
}

struct TypeDesc *
MakePrimDesc(enum BaseTypes type) {
  struct TypeDesc * desc = malloc(sizeof(struct TypeDesc));
  desc->declType = PrimType;
  desc->primDesc = type;
  return desc;
}

struct TypeDesc *
MakeFuncDesc(enum BaseTypes returnType) {
  struct TypeDesc * desc = malloc(sizeof(struct TypeDesc));
  desc->declType = FuncType;
  desc->funcDesc = malloc(sizeof(struct FuncDesc));
  desc->funcDesc->returnType = returnType;
  desc->funcDesc->funcCode = NULL;
  return desc;
}

void displayEntry(struct SymEntry * entry, int cnt, void * ignore) {
  char * scope = GetScope(GetTable(entry));
  printf("%3d %-20s %-15s",cnt,scope,GetName(entry));
  free(scope);
  int attrKind = GetAttrKind(entry);
  struct Attr * attr = GetAttr(entry);
  switch (attrKind) {
    case VOID_KIND: {
    } break;
    case INT_KIND: {
    } break;
    case STRING_KIND: {
    } break;
    case STRUCT_KIND: {
      if (attr) {
        printf("%-10s",attr->reference);
        printf("     ");
        struct TypeDesc * desc = attr->typeDesc;
        char * typeStr = StringForType(desc);
        printf("%-10s",typeStr);
        free(typeStr);
      }
      else {
        printf("empty");
      }
    } break;
  }
  printf("\n");
}

void
ListIdentifierTable() {
  printf("\nIdentifier Symbol Table\n");
  printf("Num Scope                Name           Ref       Kind Type\n");
  DoForEntries(IdentifierTable,true,displayEntry,0,NULL);
}

void
FreeTypeDesc(struct TypeDesc * desc) {
  switch (desc->declType) {
    case PrimType: {
    } break;
    case FuncType: {
      if (desc->funcDesc->funcCode) FreeSeq(desc->funcDesc->funcCode);
      free(desc->funcDesc);
    } break;
  }
  free(desc);
}

// provided to the SymTab for destroying table contents
void
FreeEntryAttr(struct SymEntry * entry, int cnt, void * ignore) {
  int attrKind = GetAttrKind(entry);
  struct Attr * attr = GetAttr(entry);
  switch (attrKind) {
    case VOID_KIND: {
    } break;
    case INT_KIND: {
    } break;
    case STRING_KIND: {
    } break;
    case STRUCT_KIND: {
      if (attr) {
        if (attr->typeDesc) FreeTypeDesc(attr->typeDesc);
        if (attr->reference) free(attr->reference);
        free(attr);
      }
    } break;
  }
}

// Semantics Actions

void
Finish() {
  printf("Finish\n");
  ListIdentifierTable();

  struct InstrSeq * textCode = GenInstr(NULL,".text",NULL,NULL,NULL);
  struct InstrSeq * dataCode = GenInstr(NULL,".data",NULL,NULL,NULL);
  // form module preamble
  struct SymEntry * mainEntry = LookupName(IdentifierTable,"main");
  if (!mainEntry) {
    PostMessageAndExit(GetCurrentColumn(),"no main function for module");
  }
  // should make sure main takes no arguments
  struct Attr * mainAttr = GetAttr(mainEntry);

  // need to keep spim happy
  AppendSeq(textCode,GenInstr(NULL,".globl","__start",NULL,NULL));
  AppendSeq(textCode,GenInstr("__start",NULL,NULL,NULL,NULL));

  AppendSeq(textCode,GenInstr(NULL,"jal",mainAttr->reference,NULL,NULL));
  AppendSeq(textCode,GenInstr(NULL,"li","$v0","10",NULL));
  AppendSeq(textCode,GenInstr(NULL,"syscall",NULL,NULL,NULL));
  // put globals in data seg
  for(int i = 0; i < IdentifierTable->size; i++){
      struct SymEntry * entry = IdentifierTable->contents[i];
      while ( entry ){
        struct Attr * attr = GetAttr(entry);
        if(attr->typeDesc->declType == PrimType){
          AppendSeq(dataCode, GenInstr(attr->reference,".word","0",NULL,NULL));
        }
        else{
          AppendSeq(textCode, attr->typeDesc->funcDesc->funcCode);
        }
        entry = entry->next;
      }
    }
  // put functions in code seq
  for(int i = 0; i < StringLiteralTable->size; i++){
    struct SymEntry * entry = StringLiteralTable->contents[i];
    while ( entry ){
      struct StringLiteral * attr = GetAttr(entry);
      //printf("\n%s\n\n",attr->name);
      AppendSeq(dataCode, attr->instrSeq);
      free(attr->name);
      free(attr);
      entry = entry->next;
    }
  }
  // combine and write
  struct InstrSeq * moduleCode = AppendSeq(textCode,dataCode);
  WriteSeq(moduleCode);

  // free code
  FreeSeq(moduleCode);
  CloseCodeGen();
  fprintf(stderr,"Finish \n");
}

void
ProcDecls(struct IdList * idList, enum BaseTypes baseType) {
  // walk IdList items
      // names on IdList are only specified as PrimType or FuncType
      // set type desc
    // global scope so will be allocated in data segment
    struct IdList * walk = idList;
    while(walk){
      struct Attr * newAttr = GetAttr(walk->entry);
      if( GetAttrKind(walk->entry) == PrimType ){
        newAttr->typeDesc = MakePrimDesc(baseType);
        char * name = malloc(sizeof(walk->entry->name));
        //remember to leave a _ before the string sequence
        sprintf(name,"_%s",walk->entry->name);
        AppendSeq(SharedSequence, GenInstr(name, ".word","0",NULL,NULL));
      }
      else{//it is a func type
        newAttr->typeDesc = MakeFuncDesc(baseType);
      }
      newAttr->reference = calloc(sizeof(walk->entry->name)+2,1);
      sprintf(newAttr->reference,"_%s",walk->entry->name);

      SetAttr(walk->entry, STRUCT_KIND, newAttr);
      walk = walk->next;
    }
  }

//join a new struct onto an existing list
struct IdList *
AppendIdList(struct IdList * item, struct IdList * list) {
  //simply add on to the list with the item
  item->next = list;
  return item;
}

struct IdList *
ProcName(char * id, enum DeclTypes type) {
  // get entry for id
  // enter id
  // create IdList node for entry
  // create and init type descriptor
  // create, init and set attr struct
  //check to see if id already exists
  if( LookupName(IdentifierTable, id)){
    printf("Entry already exists");
    return NULL;//we done
  }
// create an allocate an IdList and enter the id into the table
  struct IdList * newList = (struct IdList *)malloc(sizeof(struct IdList));
  struct SymEntry * entry = EnterName(IdentifierTable, id);
//sett the entry type and allocate an attribute
  entry->attrKind = type;
  struct Attr * attribute = (struct Attr *)malloc(sizeof(struct Attr));
//set the description for each type
  struct TypeDesc * typeDesc = (struct TypeDesc *)malloc(sizeof(struct TypeDesc));
  attribute->typeDesc = typeDesc;
  SetAttr( entry, type, attribute);
  //add the attr and entry to the newList
  newList->entry = entry;
  newList->next = NULL;
  //Remember to free!
  free(id);
  return newList;
}

void
ProcFunc(char * id, struct InstrSeq * instrs) {
  // get attr (check)
  // function entry (check)
  // include function body code (check)
  // function exit code
  struct SymEntry * entry;
  // lookup name
  entry = LookupName(IdentifierTable, id);
  if( entry ){
    struct Attr * entrAttr = GetAttr(entry);
    struct InstrSeq * instr = GenInstr(entrAttr->reference,NULL,NULL,NULL,NULL);
    AppendSeq( instr, instrs);
    //exit code
    AppendSeq(instr, GenInstr(NULL,"jr","$ra",NULL,NULL));
    entrAttr->typeDesc->funcDesc->funcCode = instr;
  }
  else{
    printf("Entry not found\n");
  }
}

//------------------------------------------------------------------------------
//Begin Semantics 2-------------------------------------------------------------
struct InstrSeq *
ProcAssign( char * id, struct ExprResult * expr){
  //remember to add a _ before the variable name
  char * varName = malloc(sizeof(id)+2);
  //sprintf is better than strcat
  sprintf(varName,"_%s",id);
  struct InstrSeq * instr = expr->instrSeq;
  //store the value of the varName in the exprResult register
  //sw $s1, $s0
  AppendSeq( instr, GenInstr(NULL,"sw",TmpRegName(expr->reg),varName,NULL));
  ReleaseTmpReg(expr->reg);
// remember to free the id
  free(id);

  return instr;
}

struct InstrSeq *
ProcPut(struct ExprResult * chars){
  struct InstrSeq * instr = chars->instrSeq;
  if( chars->type == ChrBaseType ){
    //set the call type to be 1 for a char
    AppendSeq(instr, GenInstr(NULL,"li","$v0","1",NULL));
  }
  else{//int base type
    //set the call type to be 11 for an int
    AppendSeq( instr, GenInstr(NULL,"li","$v0","11",NULL));
  }
// move the temporary S register value to an a register
  AppendSeq(instr, GenInstr(NULL,"move","$a0",TmpRegName(chars->reg),NULL));
  //syscall
  AppendSeq(instr, GenInstr(NULL,"syscall",NULL,NULL,NULL));
  //release the register to be used again
  ReleaseTmpReg(chars->reg);
  return instr;
}

struct ExprResult *
ProcAdd( struct ExprResult * r1, enum Operators operators, struct ExprResult * r2){
  //create and allocate
  struct ExprResult * finalResult = calloc(1, sizeof(struct ExprResult));
  finalResult->reg = AvailTmpReg();
  finalResult->instrSeq = calloc(1, sizeof(struct InstrSeq));
  struct InstrSeq * instr = r1->instrSeq;
  AppendSeq( instr, r2->instrSeq);
  if( operators == AddOp ){
    //generate a code sequence involving add
    // add $s3 $s1 $s2
    AppendSeq(instr, GenInstr(NULL,"add",TmpRegName(finalResult->reg),
              TmpRegName(r1->reg),TmpRegName(r2->reg)));
  }
  else if( operators == SubOp ){
    //generate a code sequence where you subtract
    // sub $s3 $s1 $s2
    AppendSeq(instr, GenInstr(NULL,"sub",TmpRegName(finalResult->reg),
              TmpRegName(r1->reg),TmpRegName(r2->reg)));

  }
  //release both registers
  ReleaseTmpReg( r1->reg );
  ReleaseTmpReg( r2->reg);
  //final result instruction sequence
  finalResult->instrSeq = instr;
  return finalResult;
}

//copy and paste ProcAdd, just change to 'mul' and 'div'
struct ExprResult *
ProcMult( struct ExprResult * r1, enum Operators operators, struct ExprResult * r2){
  struct ExprResult * finalResult = calloc(1, sizeof(struct ExprResult));
  finalResult->reg = AvailTmpReg();
  finalResult->instrSeq = calloc(1, sizeof(struct InstrSeq));
  struct InstrSeq * instr = r1->instrSeq;
  AppendSeq( instr, r2->instrSeq);

  if( operators == MultOp ){
    //generate a code sequence involving add
    // mul $s3 $s1 $s2
    AppendSeq(instr, GenInstr(NULL,"mul",TmpRegName(finalResult->reg),
              TmpRegName(r1->reg),TmpRegName(r2->reg)));
  }
  else if( operators == DivOp ){
    //generate a code sequence where you subtract
    // div $s3 $s1 $s2
    AppendSeq(instr, GenInstr(NULL,"div",TmpRegName(finalResult->reg),
              TmpRegName(r1->reg),TmpRegName(r2->reg)));

  }
  ReleaseTmpReg( r1->reg );
  ReleaseTmpReg( r2->reg);
  finalResult->instrSeq = instr;
  return finalResult;
}

struct ExprResult *
ProcNegate(struct ExprResult * expr){
  int reg1 = AvailTmpReg();
  int reg2 = AvailTmpReg();
  //move the value -1 into an available register (better than move)
  // li $s0, -1
  AppendSeq(expr->instrSeq, GenInstr(NULL,"li",TmpRegName(reg1),"-1",NULL));
  //multiply -1 by the value we have retrieved
  // mult $s2, $s0, $s1
  AppendSeq(expr->instrSeq, GenInstr(NULL,"mult",TmpRegName(reg2),TmpRegName(reg1),TmpRegName(expr->reg)));
  expr->reg = reg2;
  ReleaseTmpReg(reg1);

  return expr;
}

struct ExprResult *
ProcIntLit(char * intlit){
  struct ExprResult * result = calloc(1, sizeof(struct ExprResult));
  result->reg = AvailTmpReg();
  //move the value of the intlit into a tmp register
  result->instrSeq = GenInstr(NULL,"li", TmpRegName(result->reg),intlit,NULL);
  //set the type to be IntBaseType
  result->type = IntBaseType;
  //return the result
  return result;
}

struct ExprResult *
ProcChrLit( char * chars ){
 char charValue[4];
 struct ExprResult * result = calloc(1, sizeof(struct ExprResult));
 //this is a charbasetype
 result->type = ChrBaseType;
 //allocate an instruction sequence
 result->instrSeq = calloc(1, sizeof(struct InstrSeq));
 result->reg = AvailTmpReg();
 //the value we want is in between a ' and another ' se we want 1st position
 int val = (int) chars[1];

 sprintf(charValue, "%d", val);
//move the value of the char into a temp register
 result->instrSeq = GenInstr(NULL,"li", TmpRegName(result->reg), charValue, NULL);
//return the result
 return result;
}

struct ExprResult *
ProcId(char * id){
  //create/allocate/get a register
  struct ExprResult * result = calloc(1, sizeof(struct ExprResult));
  result->reg = AvailTmpReg();
  result->instrSeq = calloc(1, sizeof(struct InstrSeq));
//lookup the entry using the identifier table
  struct SymEntry *entry = LookupName( IdentifierTable, id );
  struct Attr * attr = GetAttr(entry);
  char * varName = malloc(sizeof(id)+2);
  sprintf(varName, "_%s", id);
//load the varName into a temporary register
  result->instrSeq = GenInstr(NULL,"lw",TmpRegName(result->reg),varName,NULL);
  free(id);
  //return the result
  return result;
}

struct ExprResult *
 ProcGet( int getType ){
   struct ExprResult * result = calloc(1, sizeof(struct ExprResult));
   result->instrSeq = calloc(1, sizeof(struct InstrSeq));
   result->reg = AvailTmpReg();

   if( getType == ChrBaseType){
     //load the char into a v register (return value from function)
     result->instrSeq = GenInstr(NULL,"li","$v0","12",NULL);
   }
   else{//IntBaseType
     //load the int into a v register
     result->instrSeq = GenInstr(NULL,"li","$v0","5",NULL);
   }
   //syscall
  AppendSeq(result->instrSeq, GenInstr(NULL,"syscall",NULL,NULL,NULL));
  //move the v register into a temp register
  AppendSeq(result->instrSeq, GenInstr(NULL,"move",TmpRegName(result->reg),"$v0",NULL));

  return result;
 }
//Begin Semantics 3---------------------------------------------------------------------
struct CondResult *
ProcCondOp(struct ExprResult *r1, enum CompareOperators cond, struct ExprResult *r2){
   char * conditionType = calloc(1,sizeof(char*));
   //set the correct condition type
   if( cond == GT ){
     conditionType = "bg";
   }
   else if( cond == GTE){
     conditionType = "bge";
   }
   else if( cond == LT){
     conditionType = "bl";
   }
   else if( cond == LTE){
     conditionType = "ble";
   }
   else if( cond == EQ){
     conditionType = "beq";
   }
   else{
     conditionType = "bne";
   }

   struct CondResult * result = calloc(1, sizeof(struct CondResult));
   result->instrSeq = r1->instrSeq;
   result->label = GenLabel();
   AppendSeq(result->instrSeq, r2->instrSeq);
   AppendSeq(result->instrSeq, GenInstr(NULL,conditionType,TmpRegName(r1->reg),
             TmpRegName(r2->reg),result->label));
   ReleaseTmpReg(r1->reg);
   ReleaseTmpReg(r2->reg);
   return result;
 }

 struct InstrSeq *
 ProcIfFunc( struct CondResult * condResult, struct InstrSeq * instrSeq ){
  struct InstrSeq * seq = condResult->instrSeq;
  AppendSeq(seq, instrSeq);
  AppendSeq(seq, GenInstr(condResult->label, NULL,NULL,NULL,NULL));
  return seq;
 }

 struct InstrSeq *
 ProcIfElseFunc( struct CondResult * condResult, struct InstrSeq * instrSeq, struct InstrSeq * elseInstrSeq ){
  char * label = GenLabel();
  struct InstrSeq * resultSeq = condResult->instrSeq;
  AppendSeq(resultSeq, instrSeq);
  AppendSeq(resultSeq, GenInstr(NULL,"b",label,NULL,NULL));
  AppendSeq(resultSeq, GenInstr(condResult->label,NULL,NULL,NULL,NULL));
  AppendSeq(resultSeq, elseInstrSeq);
  AppendSeq(resultSeq, GenInstr(label, NULL,NULL,NULL,NULL));
  free(condResult->label);
  free(condResult);
  free(label);
  return resultSeq;
 }

struct InstrSeq *
ProcWhileLoop(struct CondResult * condResult, struct InstrSeq * instrSeq){
    char * label = GenLabel();
    struct InstrSeq * result = GenInstr(label,NULL,NULL,NULL,NULL);
    AppendSeq(result, condResult->instrSeq);
    AppendSeq(result, instrSeq);
    AppendSeq(result, GenInstr(NULL,"b",label, NULL,NULL));
    AppendSeq(result, GenInstr(condResult->label ,NULL,NULL,NULL,NULL));
    free(label);
    free(condResult->label);
    free(condResult);
    return result;
}
//Begin Semantics 4-----------------------------------------------------------
char *
ProcStrLit(char * string){
  struct StringLiteral * strLiteral;
  struct SymEntry * entry = LookupName(StringLiteralTable, string);
  if(entry){
    strLiteral = GetAttr(entry);
  }
  else{
    struct SymEntry * newEntry = EnterName(StringLiteralTable, string);
    strLiteral = calloc(1,sizeof(struct StringLiteral));
    strLiteral->name = GenLabel();
    strLiteral->instrSeq = GenInstr(strLiteral->name, ".asciiz", string, NULL,NULL);
    SetAttr(newEntry, StrBaseType, strLiteral);
  }
   return strLiteral->name;
}

struct InstrSeq *
ProcPutStrLit(char * string){
  struct InstrSeq * instr = calloc(1, sizeof(struct InstrSeq));
    instr = GenInstr(NULL,"li","$v0","4",NULL);
    AppendSeq(instr, GenInstr(NULL,"la","$a0",string,NULL));
    AppendSeq(instr, GenInstr(NULL,"syscall",NULL,NULL,NULL));
    return instr;
}
