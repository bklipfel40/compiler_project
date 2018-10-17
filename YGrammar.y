%{
  #include "IOMngr.h"
  #include "Scanner.h"
  #include "YSemantics.h"
  #include <stdio.h>
  #include <string.h>

  void yyerror(char *s);
%}

/* Union structure of data that can be attached to non-terminals   */
%union     {
  int Integer;
  char * Text;
  struct IdList * IdList;
  enum BaseTypes BaseType;
  struct InstrSeq * InstrSeq;
  /* Here is my stuff below */
  struct ExprResult * ExprResult;
  enum Operators Operators;
  struct CondResult * CondResult;
  enum CompareOperators CompareOperators;
}

/* Type declaration for data attched to non-terminals. Allows      */
/* $# notation to be correctly type casted for function arguments. */
/* %type <union-field-name> non-terminal                           */
%type <Text> Id
%type <IdList> DeclList
%type <IdList> DeclItem
%type <InstrSeq> DeclImpls
%type <BaseType> Type
%type <InstrSeq> FuncBody
%type <InstrSeq> FuncStmts
%type <InstrSeq> Stmt
%type <InstrSeq> AssignStmt
/* Here is my stuff below */
%type <CondResult> Cond
%type <InstrSeq> Put
%type <InstrSeq> IfStmt
%type <InstrSeq> IfElseStmt
%type <InstrSeq> WhileLoop
%type <ExprResult> Factor
%type <ExprResult> Expr
%type <ExprResult> Term
%type <Operators> AddOp
%type <Operators> MultOp
%type <CompareOperators> Booleans
//Semantics 4
%type <Text> String




/* List of token grammar name and corresponding numbers */
/* y.tab.h will be generated from these for use by scanner*/
%token IDENT_TOK  	1
%token DECL_TOK   	2
%token IMPL_TOK   	3
/* Here is my stuff below */
//semantics 1-2
%token INT_TOK      4
%token CHR_TOK      5
%token GET_TOK      6
%token PUT_TOK      7
%token INTLIT_TOK   8
%token CHRLIT_TOK   9
//semantics 3
%token IF_TOK      10
%token ELSE_TOK    11
%token WHILE_TOK   12
%token GT_TOK      13
%token GTE_TOK     14
%token LT_TOK      15
%token LTE_TOK     16
%token EQ_TOK      17
%token NEQ_TOK     18
//semantics 4
%token STRLIT_TOK  19

// can't go past 31 without conflicting with single char tokens
// could use larger token numbers


%%
//declarations
Module        : DeclImpls                                       { Finish(); };

DeclImpls     : Decl DeclImpls                                  { };
DeclImpls     : Impl DeclImpls                                  { };
DeclImpls     :                                                 { };

Decl          : DECL_TOK DeclList ':' Type  ';'                 { ProcDecls($2,$4); };
DeclList      : DeclItem ',' DeclList                           { $$ = AppendIdList($1,$3); };
DeclList      : DeclItem                                        { $$ = $1; };

DeclItem      : Id                                              { $$ = ProcName($1,PrimType); };
DeclItem      : Id FuncArgTypes                                 { $$ = ProcName($1,FuncType); };

Id            : IDENT_TOK                                       { $$ = strdup(yytext); };

FuncArgTypes  : '('  ')'                                        {  };

Type          : INT_TOK                                         { $$ = IntBaseType; };
Type          : CHR_TOK                                         { $$ = ChrBaseType; };

Impl          : IMPL_TOK Id FuncArgNames FuncBody ';'           { ProcFunc($2,$4); };

FuncArgNames  : '('  ')'                                        {  };

FuncBody      : '{' FuncStmts '}'                               { $$ = $2; };

FuncStmts     : Stmt ';' FuncStmts                              { $$ = AppendSeq($1, $3); };
FuncStmts     :                                                 { $$ = NULL; };

Stmt          : AssignStmt                                      { $$ = $1; };

//Begin semantics1 part 2
Stmt          : Put                                             { $$ = $1; };

AssignStmt    : Id '=' Expr                                     { $$ = ProcAssign($1,$3); };

Put           : PUT_TOK '(' Expr ')'                            { $$ = ProcPut($3); };

Expr          : Expr AddOp Term                                 { $$ = ProcAdd($1, $2, $3); };
Expr          : Term                                            { $$ = $1; };

AddOp         : '+'                                             { $$ = AddOp; };
AddOp         : '-'                                             { $$ = SubOp; };

Term          : Term MultOp Factor                              { $$ = ProcMult($1, $2, $3); };
Term          : Factor                                          { $$ = $1; };

MultOp        : '*'                                             { $$ = MultOp; };
MultOp        : '/'                                             { $$ = DivOp;  };

Factor        : '(' Expr ')'                                    { $$ = $2; };
Factor        : '-' '(' Expr ')'                                { $$ = ProcNegate($3); };
Factor        : INTLIT_TOK                                      { $$ = ProcIntLit(yytext); };
Factor        : CHRLIT_TOK                                      { $$ = ProcChrLit(yytext); };
Factor        : Id                                              { $$ = ProcId($1); };
Factor        : GET_TOK '(' Type ')'                            { $$ = ProcGet($3); };

//Begin semantics 3
Stmt          : IfStmt                                          { $$ = $1; };
Stmt          : IfElseStmt                                      { $$ = $1; };
Stmt          : WhileLoop                                       { $$ = $1; };

IfStmt        : IF_TOK '(' Cond ')' FuncBody                    { $$ = ProcIfFunc($3,$5); };

IfElseStmt    : IF_TOK '(' Cond ')' FuncBody ELSE_TOK FuncBody  { $$ = ProcIfElseFunc($3,$5,$7); };

WhileLoop     : WHILE_TOK '(' Cond ')' FuncBody                 { $$ = ProcWhileLoop($3,$5);};

Cond          : Expr Booleans Expr                              { $$ = ProcCondOp($1,$2,$3); };

Booleans      : GT_TOK                                          { $$ = GT; };
Booleans      : GTE_TOK                                         { $$ = GTE;};
Booleans      : LT_TOK                                          { $$ = LT; };
Booleans      : LTE_TOK                                         { $$ = LTE;};
Booleans      : EQ_TOK                                          { $$ = EQ; };
Booleans      : NEQ_TOK                                         { $$ = NEQ;};

//Begin Semantics 4
Put           : PUT_TOK '(' String ')'                          {$$ = ProcPutStrLit($3); };

String        : STRLIT_TOK                                      {$$ = ProcStrLit(yytext); };
%%

void
yyerror( char *s)     {
  char msg[MAXLINE];
  sprintf(msg,"ERROR \"%s\" token: \"%s\"",s,yytext);
  PostMessage(GetCurrentColumn(),strlen(yytext),msg);
}
