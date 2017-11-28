/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    FLOAT_T = 258,
    INT_T = 259,
    BOOL_T = 260,
    CONST = 261,
    FALSE_C = 262,
    TRUE_C = 263,
    FUNC = 264,
    IF = 265,
    ELSE = 266,
    AND = 267,
    OR = 268,
    NEQ = 269,
    EQ = 270,
    LEQ = 271,
    GEQ = 272,
    VEC_T = 273,
    BVEC_T = 274,
    IVEC_T = 275,
    FLOAT_C = 276,
    INT_C = 277,
    ID = 278,
    UMINUS = 279,
    WITHOUT_ELSE = 280,
    WITH_ELSE = 281
  };
#endif
/* Tokens.  */
#define FLOAT_T 258
#define INT_T 259
#define BOOL_T 260
#define CONST 261
#define FALSE_C 262
#define TRUE_C 263
#define FUNC 264
#define IF 265
#define ELSE 266
#define AND 267
#define OR 268
#define NEQ 269
#define EQ 270
#define LEQ 271
#define GEQ 272
#define VEC_T 273
#define BVEC_T 274
#define IVEC_T 275
#define FLOAT_C 276
#define INT_C 277
#define ID 278
#define UMINUS 279
#define WITHOUT_ELSE 280
#define WITH_ELSE 281

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 64 "parser.y" /* yacc.c:1909  */

  int as_int;
  int as_vec;
  float as_float;
  char *as_str;
  int as_func;
  node *as_ast;

#line 115 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
