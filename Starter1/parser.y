%{
/***********************************************************************
 * --YOUR GROUP INFO SHOULD GO HERE--
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
//#include "ast.h"
//#include "symbol.h"
//#include "semantic.h"
#define YYERROR_VERBOSE
#define yTRACE(x)    { if (traceParser) fprintf(traceFile, "%s\n", x); }

void yyerror(const char* s);    /* what to do in case of error            */
int yylex();             	/* procedure for calling lexical analyzer */
extern int yyline;        	/* variable holding current line number   */

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


// TODO:Modify me to add more data types
// Can access me from flex useing yyval

%union {
	int num_int;
	float num_float;
	char *str;
}
// TODO:Replace myToken with your tokens, you can use these tokens in flex
// Variable-type tokens. Refer to "MiniGLSL language description.pdf" variable section
%token		WHITE_SPACE
%token		CONST INT_TYPE FLOAT_TYPE BOOL_TYPE VEC2_TYPE VEC3_TYPE VEC4_TYPE
%token		BVEC2_TYPE BVEC3_TYPE BVEC4_TYPE IVEC2_TYPE IVEC3_TYPE IVEC4_TYPE

// Sign-type tokens. Refer to "MiniGLSL language description.pdf" scope section
%token 		PLUS_SIGN MINUS_SIGN MULT_SIGN DIV_SIGN EXP_SIGN NOT_SIGN BITAND_SIGN BITOR_SIGN
%token		LOGICAND_SIGN LOGICOR_SIGN ISEQUAL_SIGN NOTEQUAL_SIGN LT_SIGN LE_SIGN GT_SIGN GE_SIGN
%token 		ASSIGN_SIGN

// Boolean expression tokens. Refer to "MiniGLSL language description.pdf" scope section
%token		TRUE_EXP FALSE_EXP 

// Brackets tokens
%token		LB_BRA RB_BRA LM_BRA RM_BRA LS_BRA RS_BRA

// Other tokens (comment, semicolon, etc)
%token		SEMICOLON COMMA
%token		IF ELSE FOR WHILE DO
%token		STAR_COMMENT SLASH_COMMENT
%token		INCLUDE DEFINE

// Function token
%token		FUNCTION
%token		RETURN

%token		IDENTIFIER INT_EXP FLOAT_EXP


%token 		myToken1 myToken2

%start    program

%%

/***********************************************************************
 *  Yacc/Bison rules
 *  Phase 2:
 *    1. Replace grammar found here with something reflecting the source
 *       language grammar
 *    2. Implement the trace parser option of the compiler
 *  Phase 3:
 *    1. Add code to rules for construction of AST.
 ***********************************************************************/
program
  :   tokens       
  ;
tokens
  :  tokens token  
  |      
  ;
// TODO: replace myToken with the token the you defined.
token
  :     myToken1 
  |     myToken2                     
  ;


%%

/***********************************************************************ol
 * Extra C code.
 *
 * The given yyerror function should not be touched. You may add helper
 * functions as necessary in subsequent phases.
 ***********************************************************************/
void yyerror(const char* s) {
  if (errorOccurred)
    return;    /* Error has already been reported by scanner */
  else
    errorOccurred = 1;
        
  fprintf(errorFile, "\nPARSER ERROR, LINE %d",yyline);
  if (strcmp(s, "parse error")) {
    if (strncmp(s, "parse error, ", 13))
      fprintf(errorFile, ": %s\n", s);
    else
      fprintf(errorFile, ": %s\n", s+13);
  } else
    fprintf(errorFile, ": Reading token %s\n", yytname[YYTRANSLATE(yychar)]);
}

