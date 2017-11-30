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
unsigned int regNum = 0;
regInfo *regList = NULL;


/* insert predefined variables into regList and initialize their regNames/scopeLevel/scopeIndex/next */
void initRegList() {

	regNum = 0;
	regList = NULL;

	insertPredefRegList("gl_FragColor");
	insertPredefRegList("gl_FragDepth");
	insertPredefRegList("gl_FragCoord");
	insertPredefRegList("gl_TexCoord");
	insertPredefRegList("gl_Color");
	insertPredefRegList("gl_Secondary");
	insertPredefRegList("gl_FogFragCoord");
	insertPredefRegList("gl_Light_Half");
	insertPredefRegList("gl_Light_Ambient");
	insertPredefRegList("gl_Material_Shininess");
	insertPredefRegList("env1");
	insertPredefRegList("env2");
	insertPredefRegList("env3");

	return;
}


/* helper function for initRegList() */
void insertPredefRegList(const char *name) {

	regInfo *temp = (regInfo *)malloc(sizeof(regInfo));
	const char *str = NULL;

	if(temp == NULL) {
		printf("Predefined variable's regInfo allocation has failed.\n");
		return;
	}

	if(strcmp(name, "gl_FragColor") == 0)			str = "result.color";
	else if(strcmp(name, "gl_FragDepth") == 0)		str = "result.depth";
	else if(strcmp(name, "gl_FragCoord") == 0)		str = "fragment.position";
	else if(strcmp(name, "gl_TexCoord") == 0)		str = "fragment.texcoord";
	else if(strcmp(name, "gl_Color") == 0)			str = "fragment.color";
	else if(strcmp(name, "gl_Secondary") == 0)		str = "fragment.color.secondary";
	else if(strcmp(name, "gl_FogFragCoord") == 0)		str = "fragment.fogcoord";
	else if(strcmp(name, "gl_Light_Half") == 0)		str = "state.light[0].half";
	else if(strcmp(name, "gl_Light_Ambient") == 0)		str = "state.lightmodel.ambient";
	else if(strcmp(name, "gl_Material_Shininess") == 0)	str = "state.material.shininess";
	else if(strcmp(name, "env1") == 0)			str = "program.env[1]";
	else if(strcmp(name, "env2") == 0)			str = "program.env[2]";
	else if(strcmp(name, "env3") == 0)			str = "program.env[3]";
	else {
		printf("Input predefined variable name not found. Should not be here!\n");
		free(temp);
		return;
	}
	
	sprintf(temp->regName, "%s", str);
	sprintf(temp->regNameX, "%s.x", str);
	sprintf(temp->regNameY, "%s.y", str);
	sprintf(temp->regNameZ, "%s.z", str);
	sprintf(temp->regNameW, "%s.w", str);

	sprintf(temp->varName, "%s", name);
	temp->scopeLevel = 0;
	temp->scopeIndex = 0;

	temp->next = regList;
	regList = temp;

	return;
}


/* insert varName into regList and initialize their rX/scopeLevel/scopeIndex/next */
void insertRegList(char *varName, int scopeLevel, int scopeIndex) {

	regInfo *temp = (regInfo *)malloc(sizeof(regInfo));
	unsigned int regNo = regNum++;

	if(temp == NULL) {
		printf("Predefined variable's regInfo allocation has failed.\n");
		return;
	}

	sprintf(temp->regName, "r%u", regNo);
	sprintf(temp->regNameX, "r%u.x", regNo);
	sprintf(temp->regNameY, "r%u.y", regNo);
	sprintf(temp->regNameZ, "r%u.z", regNo);
	sprintf(temp->regNameW, "r%u.w", regNo);

	sprintf(temp->varName, "%s", varName);
	temp->scopeLevel = scopeLevel;
	temp->scopeIndex = scopeIndex;

	temp->next = regList;
	regList = temp;

	return;
}


/* search varName inside regList. exist ? regName : NULL */
char *searchRegList(char *varName, int index, int scopeLevel, int scopeIndex) {

	regInfo *temp = regList;
	regInfo *found = NULL;

	while(temp != NULL) {
	
		if(strcmp(varName, temp->varName) == 0 && temp->scopeLevel == scopeLevel && temp->scopeIndex == scopeIndex) {
			found = temp;
			break;
		}
		else
			temp = temp->next;
	}

	if(found == NULL)
		return NULL;
	else
		switch(index) {
			case -1:	return found->regName;
			case 0:		return found->regNameX;
			case 1: 	return found->regNameY;
			case 2: 	return found->regNameZ;
			case 3: 	return found->regNameW;
			default: 	printf("Input vector indexing exceeds 3. You should not be here!\n");
					return NULL;
		}
}


/* free regList */
void freeRegList() {

	regInfo *temp = regList;
	while(regList != NULL) {
		temp = regList;
		regList = regList->next;
		free(temp);
	}

	return;
}


void genCode(node *ast) {


	// file create
	fp = fopen("frag.txt", "w");
	if(fp == NULL) {
		printf("Cannot open/create \"frag.txt\". Exit.\n");
		return;
	}

	// ast traverse, write instructions to file (start and end), might need dynamic allocation
//	if(genCodeRecursion(ast) < 0)
//		printf("Error when generating ARB code.\n");
	
	// file close
	fclose(fp);



	return;
}


int genCodeRecursion(node *ast) {

	// declare some temp variables to store return values from recursions
	int temp1, temp2;

  	switch(ast->kind) {

		case PROGRAM:
			temp1 = genCodeRecursion(ast->program.scope);
			return temp1;
			break;
		case SCOPE:
			temp1 = genCodeRecursion(ast->scope.declarations);
			temp2 = genCodeRecursion(ast->scope.statements);
			if(temp1 < 0) return -1;
			else if(temp2 < 0) return -1;
			else return 0;
			break;
		case DECLARATIONS:
			temp1 = genCodeRecursion(ast->declarations.declarations);
			temp2 = genCodeRecursion(ast->declarations.declaration);
			if(temp1 < 0) return -1;
			else if(temp2 < 0) return -1;
			else return temp2;
			break;
		case STATEMENTS:
			temp1 = genCodeRecursion(ast->statements.statements);
			temp2 = genCodeRecursion(ast->statements.statement);
			if(temp1 < 0) return -1;
			else if(temp2 < 0) return -1;
			else return temp2;
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
			//regNum = regNext++;
			//fprintf(fp, "TEMP r%d\n", regNum);
			// to be completed

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
			temp1 = genCodeRecursion(ast->expression_bracket.expression);
			return temp1;
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


	// need modify
	return -1;
}
