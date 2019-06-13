#include <stdbool.h>

#include "IOMngr.h"
#include "RDGrammar.h"
#include "RDTokens.h"
/*
NEED TO MATCH CURRENT TOKEN IN EMPTY CASE STATEMENTS TO PRGORESS?
check factor, term, mterm, expr, mexpr for infinite loops and
the correct select sets...
FIX TERM and possibly MEXPR and possibly MTERM
need to look at facror..?
*/

bool Prog()
// <Prog>		  :==	Ident { <StmtSeq> }
{
  ENTER;

  if (!Match(IDENT_TOK)) return LEAVE_FAIL;
  if (!Match(LBRACE_TOK)) return LEAVE_FAIL;
  if (!StmtSeq()) return LEAVE_FAIL;
  if (!Match(RBRACE_TOK)) return LEAVE_FAIL;

  return LEAVE_SUCC;
}

bool StmtSeq()
// <StmtSeq>  :==	<Stmt> ; <StmtSeq>
// <StmtSeq>	:==
{
  ENTER;

  switch (CurrentToken()) {
  case INT_TOK:
    if (!Stmt()) return LEAVE_FAIL;
    if(!Match(SEMI_TOK)) return LEAVE_FAIL;
    if(!StmtSeq()) return LEAVE_FAIL;
    break;
  case CHR_TOK:
    if (!Stmt()) return LEAVE_FAIL;
    if(!Match(SEMI_TOK)) return LEAVE_FAIL;
    if(!StmtSeq()) return LEAVE_FAIL;
    break;
	case IDENT_TOK:
    if (!Stmt()) return LEAVE_FAIL;
    if(!Match(SEMI_TOK)) return LEAVE_FAIL;
    if(!StmtSeq()) return LEAVE_FAIL;
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

/*
call match() with expected tokens for each method
may need to define certain tokens in .h file for function Calls
figure out how to write functions in notebook first
*/

bool Stmt()
// <Stmt>    :== <Decl>
// <Stmt>    :== <Assign>
{
  ENTER;
  switch (CurrentToken()) {
  case INT_TOK:
    if(!Decl()) return LEAVE_FAIL;
    break;
  case CHR_TOK:
    if(!Decl()) return LEAVE_FAIL;
    break;
  case IDENT_TOK:
    if (!Assign()) return LEAVE_FAIL;
    break;
  default:
    ParseError("No option in switch");
    return LEAVE_FAIL;
    break;
  }
  return LEAVE_SUCC;
}

bool Decl()
// <Decl>    :== <Type> <IDLst>              INT CHR
{
  ENTER;
  switch (CurrentToken()) {
    case INT_TOK:
      if (!Type()) return LEAVE_FAIL;
      if (!IDLst()) return LEAVE_FAIL;
      break;
    case CHR_TOK:
      if (!Type()) return LEAVE_FAIL;
      if (!IDLst()) return LEAVE_FAIL;
      break;
    default:
      ParseError("No option in switch");
      return LEAVE_FAIL;
      break;
  }
  return LEAVE_SUCC;
}

bool Type()
// <Type>    :== INT
// <Type>    :== CHR
{
  ENTER;
  switch (CurrentToken()) {
    case INT_TOK:
      if(!Match(INT_TOK)) return LEAVE_FAIL;
      break;
    case CHR_TOK:
      if(!Match(CHR_TOK)) return LEAVE_FAIL;
      break;
    default:
      ParseError("No option in switch");
      return LEAVE_FAIL;
      break;
  }
  return LEAVE_SUCC;
}

bool IDLst()
// <IDLst>   :== Ident <MLst>
{
  ENTER;
  switch (CurrentToken()) {
    case IDENT_TOK:
      if (!Match(IDENT_TOK)) return LEAVE_FAIL;
      if (!MLst()) return LEAVE_FAIL;
      break;
    default:
      ParseError("No option in switch");
      return LEAVE_FAIL;
      break;
  }
  return LEAVE_SUCC;
}

bool MLst()
// <MLst>    :== , <IDLst>
// <MLst>    :==
{
  ENTER;
  switch (CurrentToken()) {
    case COMMA_TOK:
      if(!Match(COMMA_TOK)) return LEAVE_FAIL;
      if(!IDLst()) return LEAVE_FAIL;
      break;
    case SEMI_TOK:
      //if(!Match(SEMI_TOK)) return LEAVE_FAIL;
      break;
    default:
      ParseError("No option in switch");
      return LEAVE_FAIL;
      break;
  }
  return LEAVE_SUCC;
}

bool Assign()
// <Assign>		:==	<Ident> := <Expr>
{
  ENTER;
  switch (CurrentToken()) {
    case IDENT_TOK:
      if (!Match(IDENT_TOK)) return LEAVE_FAIL;
      if (!Match(ASSIGN_TOK)) return LEAVE_FAIL;
      if (!Match(EQUALS_TOK)) return LEAVE_FAIL;
      if (!Expr()) return LEAVE_FAIL;
      break;
    default:
      ParseError("No option in switch");
      return LEAVE_FAIL;
      break;
  }
  return LEAVE_SUCC;
}

bool Expr()
// <Expr>		:==	<Term> <MExpr>
{
  ENTER;
  switch (CurrentToken()) {
    case LPAREN_TOK:
      if (!Term()) return LEAVE_FAIL;
      if (!MExpr()) return LEAVE_FAIL;
      //if (!Match(RPAREN_TOK)) return LEAVE_FAIL;
      break;
    case MINUS_TOK:
      if (!Term()) return LEAVE_FAIL;
      if (!MExpr()) return LEAVE_FAIL;
      break;
    case INTLIT_TOK:
      if (!Term()) return LEAVE_FAIL;
      if (!MExpr()) return LEAVE_FAIL;
      break;
    case FLOATLIT_TOK:
      if (!Term()) return LEAVE_FAIL;
      if (!MExpr()) return LEAVE_FAIL;
      break;
    case IDENT_TOK:
      if (!Term()) return LEAVE_FAIL;
      if (!MExpr()) return LEAVE_FAIL;
      break;
    default:
      ParseError("No option in switch");
      return LEAVE_FAIL;
      break;
  }
  return LEAVE_SUCC;
}

bool MExpr()
// <MExpr>		:==	<AddOp> <Term> <MExpr>
// <MExpr>		:==
{
  ENTER;
  switch (CurrentToken()) {
    case MINUS_TOK:
      if (!AddOp()) return LEAVE_FAIL;
      if(!Term()) return LEAVE_FAIL;
      if(!MExpr()) return LEAVE_FAIL;
      break;
    case PLUS_TOK:
      if (!AddOp()) return LEAVE_FAIL;
      if(!Term()) return LEAVE_FAIL;
      if(!MExpr()) return LEAVE_FAIL;
      break;
    case SEMI_TOK:
      //if(!Match(SEMI_TOK)) return LEAVE_FAIL;
      break;
    case RPAREN_TOK:
      //if(!Match(RPAREN_TOK)) return LEAVE_FAIL;
      break;
    default:
      ParseError("No option in switch");
      return LEAVE_FAIL;
      break;
  }
  return LEAVE_SUCC;
}

bool Term()
//<Term>		:==	<Factor> <MTerm>
{
  ENTER;
  switch (CurrentToken()) {
    case LPAREN_TOK:
      //if (!Match(LPAREN_TOK)) return LEAVE_FAIL;
      if (!Factor()) return LEAVE_FAIL;
      if (!MTerm()) return LEAVE_FAIL;
      //if (!Match(RPAREN_TOK)) return LEAVE_FAIL;
      break;
    case MINUS_TOK:
      if (!Factor()) return LEAVE_FAIL;
      if (!MTerm()) return LEAVE_FAIL;
      break;
    case INTLIT_TOK:
      if (!Factor()) return LEAVE_FAIL;
      if (!MTerm()) return LEAVE_FAIL;
      break;
    case FLOATLIT_TOK:
      if (!Factor()) return LEAVE_FAIL;
      if (!MTerm()) return LEAVE_FAIL;
      break;
    case IDENT_TOK:
      if (!Factor()) return LEAVE_FAIL;
      if (!MTerm()) return LEAVE_FAIL;
      break;
    default:
      ParseError("No option in switch");
      return LEAVE_FAIL;
      break;
  }
  return LEAVE_SUCC;
}

bool MTerm()
//<MTerm>		:==	<MultOp> <Factor> <MTerm>
//<MTerm>		:==
{
  ENTER;
  switch (CurrentToken()) {
    case TIMES_TOK:
      if (!MultOp()) return LEAVE_FAIL;
      if(!Factor()) return LEAVE_FAIL;
      if(!MTerm()) return LEAVE_FAIL;
      break;
    case DIV_TOK:
      if (!MultOp()) return LEAVE_FAIL;
      if(!Factor()) return LEAVE_FAIL;
      if(!MTerm()) return LEAVE_FAIL;
      break;
    case MINUS_TOK:
    //used to be matches below
      //if(!MExpr()) return LEAVE_FAIL;
      break;
    case PLUS_TOK:
      //if(!MExpr()) return LEAVE_FAIL;
      break;
    case SEMI_TOK:
      //if(!MExpr()) return LEAVE_FAIL;
      break;
    case RPAREN_TOK:
      //if(!Match(RPAREN_TOK)) return LEAVE_FAIL;
      //if(!MExpr()) return LEAVE_FAIL;
      break;
    default:
      ParseError("No option in switch");
      return LEAVE_FAIL;
      break;
  }
  return LEAVE_SUCC;
}

bool Factor()
// <Factor>	:==	( <Expr> )
// <Factor>	:==	- <Factor>
// <Factor>	:==	<IntLit>
// <Factor>	:==	<FloatLit>
// <Factor>	:==	<Ident>
{
  ENTER;
  switch (CurrentToken()) {
    case LPAREN_TOK:
      if(!Match(LPAREN_TOK)) return LEAVE_FAIL;
      if(!Expr()) return LEAVE_FAIL;
      if(!Match(RPAREN_TOK)) return LEAVE_FAIL;
      break;
    case MINUS_TOK:
      if (!Match(MINUS_TOK)) return LEAVE_FAIL;
      if(!Factor()) return LEAVE_FAIL;
      break;
    case INTLIT_TOK:
      if(!Match(INTLIT_TOK)) return LEAVE_FAIL;
      break;
    case FLOATLIT_TOK:
      if(!Match(FLOATLIT_TOK)) return LEAVE_FAIL;
      break;
    case IDENT_TOK:
      if(!Match(IDENT_TOK)) return LEAVE_FAIL;
      break;
    default:
      ParseError("No option in switch");
      return LEAVE_FAIL;
      break;
  }
  return LEAVE_SUCC;
}

bool AddOp()
// <AddOp>		:==	-
// <AddOp>		:==	+
{
  ENTER;
  switch (CurrentToken()) {
    case MINUS_TOK:
      if(!Match(MINUS_TOK)) return LEAVE_FAIL;
      break;
    case PLUS_TOK:
      if(!Match(PLUS_TOK)) return LEAVE_FAIL;
      break;
    default:
      ParseError("No option in switch");
      return LEAVE_FAIL;
      break;
  }
  return LEAVE_SUCC;
}

bool MultOp()
// <MultOp>	:==	*
// <MultOp>	:==	/
{
  ENTER;
  switch (CurrentToken()) {
    case TIMES_TOK:
      if(!Match(TIMES_TOK)) return LEAVE_FAIL;
      break;
    case DIV_TOK:
      if(!Match(DIV_TOK)) return LEAVE_FAIL;
      break;
    default:
      ParseError("No option in switch");
      return LEAVE_FAIL;
      break;
  }
  return LEAVE_SUCC;
}
