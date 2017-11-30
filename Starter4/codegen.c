/*
 *
 * Louis Chen - 1000303502
 * Juntu Chen - 1000659799
 *
 * This is the source file for ARB code generation.
 * It defines function definitions and global variables for code generation stage.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "ast.h"
#include "codegen.h"


// local variables
FILE *fp = NULL;
int regNum = -1;





void genCode(node *ast) {


	// file create
	fp = fopen("frag.txt", "w");
	if(fp == NULL) {
		printf("Cannot open/create \"frag.txt\". Exit.\n");
		return;
	}

	// ast traverse, write instructions to file (start and end), might need dynamic allocation
	regNum = -1;
//	if(genCodeRecursion(ast) < 0)
//		printf("Error when generating ARB code.\n");
	
	// file close
	fclose(fp);

	// free dynamic allocated spaces



	return;
}


int genCodeRecursion(node *ast) {

	// declare some temp variables to store return values from recursions
	int temp1, temp2;
	regNum++;

  	switch(ast->kind) {

		case PROGRAM:
			break;
		case SCOPE:
			break;
		case DECLARATIONS:
			break;
		case STATEMENTS:
			break;
		case DECLARATION:
			break;
		case DECLARATION_ASSIGN:
			break;
		case DECLARATION_ASSIGN_CONST:
			break;
		case STATEMENT_ASSIGN:
			break;
		case STATEMENT_IF_ELSE:
			break;
		case STATEMENT_IF:
			break;
		case TYPE:
			break;
		case EXPRESSION_TYPE:
			break;
		case EXPRESSION_FUNC:
			break;
		case EXPRESSION_UNARY:
			break;
		case EXPRESSION_BINARY:
			break;
		case EXPRESSION_BOOL_VALUE:
			break;
		case EXPRESSION_INT_VALUE:
			break;
		case EXPRESSION_FLOAT_VALUE:
			break;
		case EXPRESSION_BRACKET:
			break;
		case EXPRESSION_VARIABLE:
			break;
		case VARIABLE:
			break;
		case ARRAY:
			break;
		case ARGUMENTS_MORE_THAN_ONE:
			break;
		case ARGUMENTS_ONLY_ONE:
			break;
		case ARGUMENTS_OPT:
			break;
		default:
			break;
  	}



}
