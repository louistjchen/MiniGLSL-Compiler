%{
/***********************************************************************
 * --YOUR GROUP INFO SHOULD GO HERE--
 * Louis Chen - 1000303502
 * Juntu Chen - 1000659799
 *
 *   Interface to the parser module for CSC467 course project.
 * 
 *   Phase 2: Implement context free grammar for source language, and
 *            parse tracing functionality.
 *   Phase 3: Construct the AST for the source language program.
 ***********************************************************************/

/***********************************************************************
 *  C Definitions and external declarations for this module.
 *
 *  Phase 3: Include ast.h if needed, and declarations for other global or
 *           external vars, functions etc. as needed.
 ***********************************************************************/

#include <string.h>

#include "common.h"
#include "ast.h"
#include "symbol.h"
#include "semantic.h"

#define YYERROR_VERBOSE
#define yTRACE(x)    { if (traceParser) fprintf(traceFile, "%s\n", x); }

void yyerror(const char* s);    /* what to do in case of error            */
int yylex();              /* procedure for calling lexical analyzer */
extern int yyline;        /* variable holding current line number   */

enum {
  DP3 = 0, 
  LIT = 1, 
  RSQ = 2
};

%}

/***********************************************************************
 *  Yacc/Bison declarations.
 *  Phase 2:
 *    1. Add precedence declarations for operators (after %start declaration)
 *    2. If necessary, add %type declarations for some nonterminals
 *  Phase 3:
 *    1. Add fields to the union below to facilitate the construction of the
 *       AST (the two existing fields allow the lexical analyzer to pass back
 *       semantic info, so they shouldn't be touched).
 *    2. Add <type> modifiers to appropriate %token declarations (using the
 *       fields of the union) so that semantic information can by passed back
 *       by the scanner.
 *    3. Make the %type declarations for the language non-terminals, utilizing
 *       the fields of the union as well.
 ***********************************************************************/

%{
#define YYDEBUG 1
%}

// defines the yyval union
%union {
  int as_int;
  int as_vec;
  float as_float;
  char *as_str;
  int as_func;
  node *as_ast;
}

%token          FLOAT_T
%token          INT_T
%token          BOOL_T
%token          CONST
%token          FALSE_C TRUE_C
%token          FUNC
%token          IF ELSE
%token          AND OR NEQ EQ LEQ GEQ

// links specific values of tokens to yyval
%token <as_vec>   VEC_T
%token <as_vec>   BVEC_T
%token <as_vec>   IVEC_T
%token <as_float> FLOAT_C
%token <as_int>   INT_C
%token <as_str>   ID
%token <as_str>  FUNC

// operator precdence
%left     OR                        // 7
%left     AND                       // 6
%left     EQ NEQ '<' LEQ '>' GEQ    // 5
%left     '+' '-'                   // 4
%left     '*' '/'                   // 3
%right    '^'                       // 2
%right    '!' UMINUS                // 1
%left     '(' '['                   // 0

// resolve dangling else shift/reduce conflict with associativity
%left     WITHOUT_ELSE
%left     WITH_ELSE

// type declarations
// TODO: fill this out
%type <as_ast> expression
%type <as_ast> program
%type <as_ast> scope
%type <as_ast> declarations
%type <as_ast> statements
%type <as_ast> declaration
%type <as_ast> statement
%type <as_ast> type
%type <as_ast> variable
%type <as_ast> arguments_opt
%type <as_ast> arguments

// expect one shift/reduce conflict, where Bison chooses to shift
// the ELSE.
%expect 1

%start    program

%%

/***********************************************************************
 *  Yacc/Bison rules
 *  Phase 2:
 *    1. Replace grammar found here with something reflecting the source
 *       language grammar
 *    2. Implement the trace parser option of the compiler
 ***********************************************************************/
program
  : scope 
      { yTRACE("program -> scope\n");
	ast = ast_allocate(PROGRAM, $1, yyline); }
  ;

scope
  : '{' declarations statements '}'
      { yTRACE("scope -> { declarations statements }\n");
	$$ = ast_allocate(SCOPE, $2, $3, yyline); }
  ;

declarations
  : declarations declaration
      { yTRACE("declarations -> declarations declaration\n");
	$$ = ast_allocate(DECLARATIONS, $1, $2, yyline); }
  | 
      { yTRACE("declarations -> \n") 
	$$ = NULL; }
  ;

statements
  : statements statement
      { yTRACE("statements -> statements statement\n");
	$$ = ast_allocate(STATEMENTS, $1, $2, yyline); }
  | 
      { yTRACE("statements -> \n");
	$$ = NULL; }
  ;

declaration
  : type ID ';' 
      { yTRACE("declaration -> type ID ;\n");
	$$ = ast_allocate(DECLARATION, $1, $2, yyline); }
  | type ID '=' expression ';'
      { yTRACE("declaration -> type ID = expression ;\n");
	$$ = ast_allocate(DECLARATION_ASSIGN, $1, $2, $4, yyline); }
  | CONST type ID '=' expression ';'
      { yTRACE("declaration -> CONST type ID = expression ;\n");
	$$ = ast_allocate(DECLARATION_ASSIGN_CONST, $2, $3, $5, yyline); }
  ;

statement
  : variable '=' expression ';'
      { yTRACE("statement -> variable = expression ;\n");
	$$ = ast_allocate(STATEMENT_ASSIGN, $1, $3, yyline); }
  | IF '(' expression ')' statement ELSE statement %prec WITH_ELSE
      { yTRACE("statement -> IF ( expression ) statement ELSE statement \n");
	$$ = ast_allocate(STATEMENT_IF_ELSE, $3, $5, $7, yyline); }
  | IF '(' expression ')' statement %prec WITHOUT_ELSE
      { yTRACE("statement -> IF ( expression ) statement \n");
	$$ = ast_allocate(STATEMENT_IF, $3, $5, yyline); }
  | scope 
      { yTRACE("statement -> scope \n");
	$$ = ast_allocate(PROGRAM, $1, yyline); }
  | ';'
      { yTRACE("statement -> ; \n");
	$$ = NULL; }
  ;

type
  : INT_T
      { yTRACE("type -> INT_T \n");
	$$ = ast_allocate(TYPE, INT, yyline); }
  | IVEC_T
      { yTRACE("type -> IVEC_T \n");
	$$ = ast_allocate(TYPE, $1+IVEC2-1, yyline); }
  | BOOL_T
      { yTRACE("type -> BOOL_T \n");
	$$ = ast_allocate(TYPE, BOOL, yyline); }
  | BVEC_T
      { yTRACE("type -> BVEC_T \n");
	$$ = ast_allocate(TYPE, $1+BVEC2-1, yyline); }
  | FLOAT_T
      { yTRACE("type -> FLOAT_T \n");
	$$ = ast_allocate(TYPE, FLOAT, yyline); }
  | VEC_T
      { yTRACE("type -> VEC_T \n");
	$$ = ast_allocate(TYPE, $1+VEC2-1, yyline); }
  ;


expression

  /* function-like operators */
  : type '(' arguments_opt ')' %prec '('
      { yTRACE("expression -> type ( arguments_opt ) \n");
	$$ = ast_allocate(EXPRESSION_TYPE, $1, $3, yyline); }
  | FUNC '(' arguments_opt ')' %prec '('
      { yTRACE("expression -> FUNC ( arguments_opt ) \n");
	$$ = ast_allocate(EXPRESSION_FUNC, $1, $3, yyline); }

  /* unary opterators */
  | '-' expression %prec UMINUS
      { yTRACE("expression -> - expression \n");
	$$ = ast_allocate(EXPRESSION_UNARY, UNARY_NEGATIVE, $2, yyline); }
  | '!' expression %prec '!'
      { yTRACE("expression -> ! expression \n");
	$$ = ast_allocate(EXPRESSION_UNARY, UNARY_EXCLAMATION, $2, yyline); }

  /* binary operators */
  | expression AND expression %prec AND
      { yTRACE("expression -> expression AND expression \n");
	$$ = ast_allocate(EXPRESSION_BINARY, BINARY_AND, $1, $3, yyline); }
  | expression OR expression %prec OR
      { yTRACE("expression -> expression OR expression \n")
	$$ = ast_allocate(EXPRESSION_BINARY, BINARY_OR, $1, $3, yyline); }
  | expression EQ expression %prec EQ
      { yTRACE("expression -> expression EQ expression \n")
	$$ = ast_allocate(EXPRESSION_BINARY, BINARY_EQ, $1, $3, yyline); }
  | expression NEQ expression %prec NEQ
      { yTRACE("expression -> expression NEQ expression \n")
	$$ = ast_allocate(EXPRESSION_BINARY, BINARY_NEQ, $1, $3, yyline); }
  | expression '<' expression %prec '<'
      { yTRACE("expression -> expression < expression \n")
	$$ = ast_allocate(EXPRESSION_BINARY, BINARY_LT, $1, $3, yyline); }
  | expression LEQ expression %prec LEQ
      { yTRACE("expression -> expression LEQ expression \n")
	$$ = ast_allocate(EXPRESSION_BINARY, BINARY_LEQ, $1, $3, yyline); }
  | expression '>' expression %prec '>'
      { yTRACE("expression -> expression > expression \n")
	$$ = ast_allocate(EXPRESSION_BINARY, BINARY_GT, $1, $3, yyline); }
  | expression GEQ expression %prec GEQ
      { yTRACE("expression -> expression GEQ expression \n")
	$$ = ast_allocate(EXPRESSION_BINARY, BINARY_GEQ, $1, $3, yyline); }
  | expression '+' expression %prec '+'
      { yTRACE("expression -> expression + expression \n")
	$$ = ast_allocate(EXPRESSION_BINARY, BINARY_PLUS, $1, $3, yyline); }
  | expression '-' expression %prec '-'
      { yTRACE("expression -> expression - expression \n")
	$$ = ast_allocate(EXPRESSION_BINARY, BINARY_MINUS, $1, $3, yyline); }
  | expression '*' expression %prec '*'
      { yTRACE("expression -> expression * expression \n")
	$$ = ast_allocate(EXPRESSION_BINARY, BINARY_TIMES, $1, $3, yyline); }
  | expression '/' expression %prec '/'
      { yTRACE("expression -> expression / expression \n")
	$$ = ast_allocate(EXPRESSION_BINARY, BINARY_DIVIDE, $1, $3, yyline); }
  | expression '^' expression %prec '^'
      { yTRACE("expression -> expression ^ expression \n")
	$$ = ast_allocate(EXPRESSION_BINARY, BINARY_XOR, $1, $3, yyline); }

  /* literals */
  | TRUE_C
      { yTRACE("expression -> TRUE_C \n");
	$$ = ast_allocate(EXPRESSION_BOOL_VALUE, 1, yyline); }
  | FALSE_C
      { yTRACE("expression -> FALSE_C \n");
	$$ = ast_allocate(EXPRESSION_BOOL_VALUE, 0, yyline); }
  | INT_C
      { yTRACE("expression -> INT_C \n");
	$$ = ast_allocate(EXPRESSION_INT_VALUE, $1, yyline); }
  | FLOAT_C
      { yTRACE("expression -> FLOAT_C \n")
	$$ = ast_allocate(EXPRESSION_FLOAT_VALUE, $1, yyline); }

  /* misc */
  | '(' expression ')'
      { yTRACE("expression -> ( expression ) \n");
	$$ = ast_allocate(EXPRESSION_BRACKET, $2, yyline); }
  | variable
    { yTRACE("expression -> variable \n");
	$$ = ast_allocate(EXPRESSION_VARIABLE, $1, yyline); }
  ;

variable
  : ID
      { yTRACE("variable -> ID \n");
	$$ = ast_allocate(VARIABLE, $1, yyline); }
  | ID '[' INT_C ']' %prec '['
      { yTRACE("variable -> ID [ INT_C ] \n");
	$$ = ast_allocate(ARRAY, $1, $3, yyline); }
  ;

arguments
  : arguments ',' expression
      { yTRACE("arguments -> arguments , expression \n");
	$$ = ast_allocate(ARGUMENTS_MORE_THAN_ONE, $1, $3, yyline); }
  | expression
      { yTRACE("arguments -> expression \n");
	$$ = ast_allocate(ARGUMENTS_ONLY_ONE, $1, yyline); }
  ;

arguments_opt
  : arguments
      { yTRACE("arguments_opt -> arguments \n");
	$$ = ast_allocate(ARGUMENTS_OPT, $1, yyline); }
  |
      { yTRACE("arguments_opt -> \n");
	$$ = NULL; }
  ;

%%

/***********************************************************************ol
 * Extra C code.
 *
 * The given yyerror function should not be touched. You may add helper
 * functions as necessary in subsequent phases.
 ***********************************************************************/
void yyerror(const char* s) {
  if(errorOccurred) {
    return;    /* Error has already been reported by scanner */
  } else {
    errorOccurred = 1;
  }

  fprintf(errorFile, "\nPARSER ERROR, LINE %d", yyline);
  
  if(strcmp(s, "parse error")) {
    if(strncmp(s, "parse error, ", 13)) {
      fprintf(errorFile, ": %s\n", s);
    } else {
      fprintf(errorFile, ": %s\n", s+13);
    }
  } else {
    fprintf(errorFile, ": Reading token %s\n", yytname[YYTRANSLATE(yychar)]);
  }
}

