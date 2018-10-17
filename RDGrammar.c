#include <stdbool.h>

#include "IOMngr.h"
#include "RDGrammar.h"
#include "RDTokens.h"
/*
 * Author:  Ben Klipfel
 * Date:    11/1/2017
 * Resources: Kala Arentz
 */

bool Prog()
// <Prog>		  :==	Ident { <StmtSeq> }
{
  ENTER;

  if (!Match(IDENT_TOK)){ return LEAVE_FAIL; }
  if (!Match(LBRACE_TOK)){ return LEAVE_FAIL;}
  if (!StmtSeq()){ return LEAVE_FAIL;}
  if (!Match(RBRACE_TOK)){ return LEAVE_FAIL;}

  return LEAVE_SUCC;
}

bool StmtSeq()
// <StmtSeq>  :==	<Stmt> ; <StmtSeq>
// <StmtSeq>	:==
{
  ENTER;
  switch (CurrentToken()) {
  case INT_TOK:
  case CHR_TOK:
	case IDENT_TOK:
    if( !Stmt() ){ return LEAVE_FAIL; }
    if( !Match(SEMI_TOK) ){ return LEAVE_FAIL; }
    if( !StmtSeq()){ return LEAVE_FAIL; }
	  break;
	case RBRACE_TOK:
	  break;
	default:
    ParseError("No option in switch");
	  return LEAVE_FAIL;
		break;
	}
	return LEAVE_SUCC;
}
bool Stmt()
// <Stmt>    :== <Decl>
// <Stmt>    :== <Assign>
{
  ENTER;

  switch( CurrentToken() ){
    case INT_TOK:
    case CHR_TOK:
      if(!Decl() ){ return LEAVE_FAIL;}
      break;
    case IDENT_TOK:
      if(!Assign() ){ return LEAVE_FAIL;}
      break;
    default:
      ParseError("No option in switch");
      return LEAVE_FAIL;
  }
  return LEAVE_SUCC;
}

bool Decl()
// <Decl>    :== <Type> <IDLst>              INT CHR
{
  ENTER;

  if( !Type() ){ return LEAVE_FAIL; }
  if( !IDLst() ){ return LEAVE_FAIL; }

  return LEAVE_SUCC;
}

bool Type()
// <Type>    :== INT
// <Type>    :== CHR
{
  ENTER;

  switch( CurrentToken() ){
    case INT_TOK:
      if(!Match(INT_TOK)){ return LEAVE_FAIL; }
      break;
    case CHR_TOK:
      if(!Match(CHR_TOK)){ return LEAVE_FAIL; }
      break;
    default:
      ParseError("No option in switch");
      return LEAVE_FAIL;
  }

  return LEAVE_SUCC;
}

bool IDLst()
// <IDLst>   :== Ident <MLst>
{
  ENTER;
  if( !Match(IDENT_TOK)){ return LEAVE_FAIL; }
  if( !MLst() ){ return LEAVE_FAIL; }
  return LEAVE_SUCC;
}

bool MLst()
// <MLst>    :== , <IDLst>
// <MLst>    :==
{
  ENTER;

  switch( CurrentToken() ){
    case COMMA_TOK:
      if(!Match(COMMA_TOK)){ return LEAVE_FAIL; }
      if(!IDLst() ){ return LEAVE_FAIL; }
      break;
    case RPAREN_TOK:
    case SEMI_TOK:
      break;
    default:
      ParseError("No option in switch");
      return LEAVE_FAIL;
  }
  return LEAVE_SUCC;
}

bool Assign()
// <Assign>		:==	<Ident> := <Expr>
{
  ENTER;
  if( !Match(IDENT_TOK) ){ return LEAVE_FAIL; }
  if( !Match(ASSIGN_TOK) ){ return LEAVE_FAIL; }
  if( !Expr() ){ return LEAVE_FAIL; }
  return LEAVE_SUCC;
}

bool Expr()
// <Expr>		:==	<Term> <MExpr>
{
  ENTER;
  //check for term, MExpr
  if( !Term() ){ return LEAVE_FAIL; }
  if( !MExpr() ){ return LEAVE_FAIL; }
  return LEAVE_SUCC;
}

bool MExpr()
// <MExpr>		:==	<AddOp> <Term> <MExpr>
// <MExpr>		:==
{
  ENTER;
  //default is LEAVE_SUCC because grammar goes empty
  switch( CurrentToken() ){
    case PLUS_TOK:
    case MINUS_TOK:
      if( !AddOp() ) { return LEAVE_FAIL; }
      if( !Term() )  { return LEAVE_FAIL; }
      if( !MExpr() ) { return LEAVE_FAIL; }
      break;
    default:
      return LEAVE_SUCC;
  }
  return LEAVE_SUCC;
}

bool Term()
//<Term>		:==	<Factor> <MTerm>
{
  ENTER;
  if( !Factor() ){ return LEAVE_FAIL; }
  if( !MTerm() ){ return LEAVE_FAIL; }
  return LEAVE_SUCC;
}

bool MTerm()
//<MTerm>		:==	<MultOp> <Factor> <MTerm>
//<MTerm>		:==
{
  ENTER;

  switch( CurrentToken() ){
    case TIMES_TOK:
    case DIV_TOK:
      if(!MultOp() ){ return LEAVE_FAIL;}
      if(!Factor() ){ return LEAVE_FAIL;}
      if(!MTerm()  ){ return LEAVE_FAIL;}
      break;
    case PLUS_TOK:
    case MINUS_TOK:
    case SEMI_TOK:
    case RPAREN_TOK:
      break;
    default:
      ParseError("No option in switch");
      return LEAVE_FAIL;
  }
  return LEAVE_SUCC;
}

bool Factor()
// <Factor>	:==	( <Expr> )
// <Factor>	:==	- <Factor>
// <Factor>	:==	<IntLit>
// <Factor>	:==	<RealLit>
// <Factor>	:==	<Ident>
{
  ENTER;
  switch( CurrentToken() ){
    case LPAREN_TOK:
      if(!Match( LPAREN_TOK) ){ return LEAVE_FAIL;}
      if(!Expr() ){ return LEAVE_FAIL;}
      if(!Match( RPAREN_TOK ) ){ return LEAVE_FAIL;}
      break;
    case MINUS_TOK:
      if(!Match(MINUS_TOK)){ return LEAVE_FAIL;}
      if(!Factor() ){ return LEAVE_FAIL;}
      break;
    case INTLIT_TOK:
      if(!Match(INTLIT_TOK) ){ return LEAVE_FAIL;}
      break;
    case REALLIT_TOK:
      if( !Match(REALLIT_TOK) ){ return LEAVE_FAIL;}
      break;
    case IDENT_TOK:
      if( !Match(IDENT_TOK) ){ return LEAVE_FAIL;}
      break;
      case PLUS_TOK:
        if(!Match(PLUS_TOK)){return LEAVE_FAIL;}
        if(!Factor() ){ return LEAVE_FAIL;}
        break;
    default:
      ParseError("No option in switch");
      return LEAVE_FAIL;
  }
  return LEAVE_SUCC;
}

bool AddOp()
// <AddOp>		:==	-
// <AddOp>		:==	+
{
  ENTER;

  switch( CurrentToken() ){
    case MINUS_TOK:
      if( !Match(MINUS_TOK) ) return LEAVE_FAIL;
      break;
    case PLUS_TOK:
      if( !Match(PLUS_TOK) ) return LEAVE_FAIL;
      break;
    default:
      ParseError("No option in this switch");
      return LEAVE_FAIL;
  }
  return LEAVE_SUCC;
}

bool MultOp()
// <MultOp>	:==	*
// <MultOp>	:==	/
{
 ENTER;
 switch( CurrentToken() ){
   case TIMES_TOK:
      if( !Match(TIMES_TOK) ) return LEAVE_FAIL;
      break;
  case DIV_TOK:
      if(!Match(DIV_TOK) ) return LEAVE_FAIL;
      break;
  default:
      ParseError("No option in this switch");
      return LEAVE_FAIL;
  }
  return LEAVE_SUCC;
 }
