/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IDENT_TOK = 1,
     DECL_TOK = 2,
     IMPL_TOK = 3,
     INT_TOK = 4,
     CHR_TOK = 5,
     GET_TOK = 6,
     PUT_TOK = 7,
     INTLIT_TOK = 8,
     CHRLIT_TOK = 9,
     IF_TOK = 10,
     ELSE_TOK = 11,
     WHILE_TOK = 12,
     GT_TOK = 13,
     GTE_TOK = 14,
     LT_TOK = 15,
     LTE_TOK = 16,
     EQ_TOK = 17,
     NEQ_TOK = 18,
     STRLIT_TOK = 19
   };
#endif
/* Tokens.  */
#define IDENT_TOK 1
#define DECL_TOK 2
#define IMPL_TOK 3
#define INT_TOK 4
#define CHR_TOK 5
#define GET_TOK 6
#define PUT_TOK 7
#define INTLIT_TOK 8
#define CHRLIT_TOK 9
#define IF_TOK 10
#define ELSE_TOK 11
#define WHILE_TOK 12
#define GT_TOK 13
#define GTE_TOK 14
#define LT_TOK 15
#define LTE_TOK 16
#define EQ_TOK 17
#define NEQ_TOK 18
#define STRLIT_TOK 19




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 12 "YGrammar.y"
{
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
/* Line 1529 of yacc.c.  */
#line 104 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

