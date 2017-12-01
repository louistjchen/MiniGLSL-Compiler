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

#include "common.h"
#include "ast.h"
#include "symbol.h"
#include "semantic.h"
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
	temp->ast = NULL;

	temp->next = regList;
	regList = temp;

	return;
}


/* create a regInfo for a variable in regList and initialize its information */
char *insertVarRegList(char *varName, int scopeLevel, int scopeIndex) {

	regInfo *temp = (regInfo *)malloc(sizeof(regInfo));
	unsigned int regNo;

	if(temp == NULL) {
		printf("Variable's regInfo allocation has failed.\n");
		return NULL;
	}

	regNo = regNum++;

	sprintf(temp->regName, "r%uvar", regNo);
	sprintf(temp->regNameX, "r%uvar.x", regNo);
	sprintf(temp->regNameY, "r%uvar.y", regNo);
	sprintf(temp->regNameZ, "r%uvar.z", regNo);
	sprintf(temp->regNameW, "r%uvar.w", regNo);

	sprintf(temp->varName, "%s", varName);
	temp->scopeLevel = scopeLevel;
	temp->scopeIndex = scopeIndex;
	temp->ast = NULL;

	temp->next = regList;
	regList = temp;

	return temp->regName;
}


/* create a regInfo for an expression in regList and initialize its information */
char *insertExpRegList(node *ast) {

	regInfo *temp = (regInfo *)malloc(sizeof(regInfo));
	unsigned int regNo;

	if(temp == NULL) {
		printf("Expression's regInfo allocation has failed.\n");
		return NULL;
	}

	regNo = regNum++;

	sprintf(temp->regName, "r%utmp", regNo);
	sprintf(temp->regNameX, "r%utmp.x", regNo);
	sprintf(temp->regNameY, "r%utmp.y", regNo);
	sprintf(temp->regNameZ, "r%utmp.z", regNo);
	sprintf(temp->regNameW, "r%utmp.w", regNo);

	sprintf(temp->varName, "__EXPRESSION__");
	temp->scopeLevel = ast->scopeLevel;
	temp->scopeIndex = ast->scopeIndex;
	temp->ast = ast;

	temp->next = regList;
	regList = temp;

	return temp->regName;
}


/* search varName inside regList. EXIST ? regName : NULL */
char *searchVarRegList(node *ast, int index) {

	regInfo *temp = regList;
	regInfo *found = NULL;

	char *varName;
	int scopeLevel;
	int scopeIndex;

	L_node* Cur = NULL;
	node* Temp = NULL;
	L_node* Target = NULL;
	
	Cur = Head;

	if(ast->kind == VARIABLE) {
	
		// First, find the same name, parent/current scope, smaller ln number variable in symbol table.
		while(Cur != NULL){
			Temp = ast;
			if((strcmp(Cur->Name, ast->variable.identifier)) == 0 && (Cur->Scope <= ast->scopeLevel) && (Cur->Line_num <= ast->variable.ln)){
				// At parent Scopelevel
				if(Cur->Scope < ast->scopeLevel){
					// Find which parent node is that "parent"
					while(Temp != NULL){
						// Found the parent scope.
						if(Cur->Scope == Temp->scopeLevel && Cur->Test == Temp->scopeIndex){
							Target = Cur;
							break;
						}
						Temp = Temp->parent;
					}				
				}
			
				// At the same Scopelevel
				else if(Cur->Scope == ast->scopeLevel){
					// Fouond the same level and same index
					if(Cur->Test == ast->scopeIndex){
						Target = Cur;
					}
				}
				else{
					printf("You shouldn't be here. Get out!\n");
					return NULL;
				}	
			}
			if(Target != NULL)
				break;
			Cur = Cur->Next;
		}
	}
	else if(ast->kind == ARRAY) {
	
		// First, find the same name, parent/current scope, smaller ln number array in symbol table.
		while(Cur != NULL){
			Temp = ast;
			if((strcmp(Cur->Name, ast->array.identifier)) == 0 && (Cur->Scope <= ast->scopeLevel) && (Cur->Line_num <= ast->array.ln)){
				// At parent Scopelevel
				if(Cur->Scope < ast->scopeLevel){
					// Find which parent node is that "parent"
					while(Temp != NULL){
						// Found the parent scope.
						if(Cur->Scope == Temp->scopeLevel && Cur->Test == Temp->scopeIndex){
							Target = Cur;
							break;
						}
						Temp = Temp->parent;
					}				
				}
			
				// At the same Scopelevel
				else if(Cur->Scope == ast->scopeLevel){
					// Fouond the same level and same index
					if(Cur->Test == ast->scopeIndex){
						Target = Cur;
					}
				}
				else{
					printf("You shouldn't be here. Get out!\n");
					return NULL;
				}	
			}
			if(Target != NULL)
				break;
			Cur = Cur->Next;
		}
	}
	else {	
		printf("AST node is not a variable/array node.\n");
		return NULL;
	}

	
	if(Target == NULL) {
		printf("Searching the regInfo that has correct scopeLevel/scopeIndex as the input variable. You should not be here!\n");
		return NULL;
	}
	else {
		scopeLevel = Target->Scope;
		scopeIndex = Target->Test;
	}

	if(ast->kind == VARIABLE)
		varName = ast->variable.identifier;
	else if(ast->kind == ARRAY)
		varName = ast->array.identifier;
	else {
		printf("Searching the regInfo that has correct scopeLevel/scopeIndex as the input variable. You should not be here!\n");
		return NULL;
	}

	while(temp != NULL) {
	
		if(strcmp(varName, temp->varName) == 0 && temp->scopeLevel == scopeLevel && temp->scopeIndex == scopeIndex) {
			found = temp;
			break;
		}
		else
			temp = temp->next;
	}

	if(found == NULL) {
		printf("Searching the regInfo that has correct scopeLevel/scopeIndex as the input variable. You should not be here!\n");
		return NULL;
	}
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


/* search expression given its ast inside regList. EXIST ? regName : NULL */
char *searchExpRegList(node *ast) {

	regInfo *temp = regList;
	regInfo *found = NULL;

	if(ast == NULL) {
		printf("Function \"searchExpRegList\" has failed. Input ast pointer is set to NULL.\n");
		return NULL;
	}

	while(temp != NULL) {
		if(ast == temp->ast) {
			found = temp;
			break;
		}
		else
			temp = temp->next;
	}

	if(found == NULL)
		return NULL;
	else
		return found->regName;
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


	// copy file pointer; no need to close file as compiler.c does it
	fp = outputFile;
	if(fp == NULL) {
		printf("Cannot open/create \"frag.txt\". Exit.\n");
		return;
	}

	// ast traverse, write instructions to file (start and end)
	fprintf(fp, "!!ARBfp1.0\n\n");
	genCodeRecursion(ast);
	fprintf(fp, "\nEND\n");
	
	return;
}


void genCodeRecursion(node *ast) {

	// declare some temp variables to store return values from recursions
	char *tempReg1;
	char *tempReg2;
	char *tempReg3;
	int numArgs;

	// return when reaching empty node
	if(ast == NULL)
		return;

  	switch(ast->kind) {

		case PROGRAM:
			genCodeRecursion(ast->program.scope);
			return;
		case SCOPE:
			genCodeRecursion(ast->scope.declarations);
			genCodeRecursion(ast->scope.statements);
			return;
		case DECLARATIONS:
			genCodeRecursion(ast->declarations.declarations);
			genCodeRecursion(ast->declarations.declaration);
			return;
		case STATEMENTS:
			genCodeRecursion(ast->statements.statements);
			genCodeRecursion(ast->statements.statement);
			return;
		case DECLARATION:
			tempReg1 = insertVarRegList(ast->declaration.identifier, ast->scopeLevel, ast->scopeIndex);
			fprintf(fp, "TEMP %s;\n", tempReg1);
			return;
		case DECLARATION_ASSIGN:
			tempReg1 = insertVarRegList(ast->declaration_assign.identifier, ast->scopeLevel, ast->scopeIndex);
			genCodeRecursion(ast->declaration_assign.expression);
			tempReg2 = searchExpRegList(ast->declaration_assign.expression);
			fprintf(fp, "TEMP %s;\n", tempReg1);
			fprintf(fp, "MOV %s, %s;\n", tempReg1, tempReg2);
			return;
		case DECLARATION_ASSIGN_CONST:
			tempReg1 = insertVarRegList(ast->declaration_assign_const.identifier, ast->scopeLevel, ast->scopeIndex);
			genCodeRecursion(ast->declaration_assign_const.expression);
			tempReg2 = searchExpRegList(ast->declaration_assign_const.expression);
			fprintf(fp, "PARAM %s = %s;\n", tempReg1, tempReg2);
			return;
		case STATEMENT_ASSIGN:
			if(ast->statement_assign.variable->kind == VARIABLE)
				tempReg1 = searchVarRegList(ast->statement_assign.variable, -1);
			else if(ast->statement_assign.variable->kind == ARRAY)
				tempReg1 = searchVarRegList(ast->statement_assign.variable, ast->statement_assign.variable->array.index);
			else {
				printf("Error: genCodeRecursion() case: STATEMENT_ASSIGN. You should not be here!\n");
				return;
			}
			genCodeRecursion(ast->statement_assign.expression);
			tempReg2 = searchExpRegList(ast->statement_assign.expression);
			fprintf(fp, "MOV %s, %s;\n", tempReg1, tempReg2);
			return;
		case STATEMENT_IF_ELSE://
			return;
		case STATEMENT_IF://
			return;
		case TYPE:
			printf("Error: genCodeRecursion() case: TYPE. You should not be here!\n");
			return;
		case EXPRESSION_TYPE:
			tempReg1 = insertExpRegList(ast);
			genCodeRecursion(ast->expression_type.arguments);
			tempReg2 = searchExpRegList(ast->expression_type.arguments);
			numArgs = numArguments(ast->expression_type.arguments);
			switch(numArgs) {
				case 1:
					fprintf(fp, "TEMP %s;\n", tempReg1);
					fprintf(fp, "MOV %s.x, %s.x;\n", tempReg1, tempReg2);
					break;
				case 2:
					fprintf(fp, "TEMP %s;\n", tempReg1);
					fprintf(fp, "MOV %s.x, %s.x;\n", tempReg1, tempReg2);
					fprintf(fp, "MOV %s.y, %s.y;\n", tempReg1, tempReg2);
					break;
				case 3:
					fprintf(fp, "TEMP %s;\n", tempReg1);
					fprintf(fp, "MOV %s.x, %s.x;\n", tempReg1, tempReg2);
					fprintf(fp, "MOV %s.y, %s.y;\n", tempReg1, tempReg2);
					fprintf(fp, "MOV %s.z, %s.z;\n", tempReg1, tempReg2);
					break;
				case 4:
					fprintf(fp, "TEMP %s;\n", tempReg1);
					fprintf(fp, "MOV %s.x, %s.x;\n", tempReg1, tempReg2);
					fprintf(fp, "MOV %s.y, %s.y;\n", tempReg1, tempReg2);
					fprintf(fp, "MOV %s.z, %s.z;\n", tempReg1, tempReg2);
					fprintf(fp, "MOV %s.w, %s.w;\n", tempReg1, tempReg2);
					break;
				default:
					printf("Error: genCodeRecursion() case: EXPRESSION_TYPE. You should not be here!\n");
					break;
			}
			return;
		case EXPRESSION_FUNC:
			tempReg1 = insertExpRegList(ast);
			genCodeRecursion(ast->expression_func.arguments);
			tempReg2 = searchExpRegList(ast->expression_func.arguments);
			switch(ast->expression_func.function) {
				case 0: // DP3
//					fprintf(fp, "TEMP %s;\n", tempReg1);
//					fprintf(fp, "DP3 %s, %s, %s");
					break;
				case 1: // LIT
					break;
				case 2: // RSQ
					fprintf(fp, "TEMP %s;\n", tempReg1);
					fprintf(fp, "RSQ %s, %s;\n", tempReg1, tempReg2);
					break;
				default:
					printf("Error: genCodeRecursion() case: EXPRESSION_FUNC. You should not be here!\n");
					break;
			}
			return;
		case EXPRESSION_UNARY:
			tempReg1 = insertExpRegList(ast);
			genCodeRecursion(ast->expression_unary.right);
			tempReg2 = searchExpRegList(ast->expression_unary.right);
			switch(ast->expression_unary.op) {
				case UNARY_NEGATIVE: // for arithmetic
					fprintf(fp, "TEMP %s;\n", tempReg1);
					fprintf(fp, "MUL %s, %s, -1.0;\n", tempReg1, tempReg2);
					break;
				case UNARY_EXCLAMATION: // for boolean
					fprintf(fp, "TEMP %s;\n", tempReg1);
					fprintf(fp, "SUB %s, 1.0, %s;\n", tempReg1, tempReg2);
					break;
				default:
					printf("Error: genCodeRecursion() case: EXPRESSION_UNARY. You should not be here!\n");
					break;
			}
			return;
		case EXPRESSION_BINARY:
			tempReg1 = insertExpRegList(ast);
			genCodeRecursion(ast->expression_binary.left);
			tempReg2 = searchExpRegList(ast->expression_binary.left);
			genCodeRecursion(ast->expression_binary.right);
			tempReg3 = searchExpRegList(ast->expression_binary.right);
			switch(ast->expression_binary.op) {
				case BINARY_AND:
					fprintf(fp, "TEMP %s;\n", tempReg1);
					fprintf(fp, "MUL %s, %s, %s;\n", tempReg1, tempReg2, tempReg3);
					break;
				case BINARY_OR:
					fprintf(fp, "TEMP %s;\n", tempReg1);
					fprintf(fp, "ADD %s, %s, %s;\n", tempReg1, tempReg2, tempReg3);
					fprintf(fp, "SUB %s, %s, 1.0;\n", tempReg1, tempReg1);
					fprintf(fp, "CMP %s, %s, {0.0, 0.0, 0.0, 0.0}, {1.0, 1.0, 1.0, 1.0};\n", tempReg1, tempReg1);
					break;
				case BINARY_EQ:
					fprintf(fp, "TEMP %s;\n", tempReg1);
					fprintf(fp, "SUB %s, %s, %s;\n", tempReg1, tempReg2, tempReg3);
					fprintf(fp, "SUB %s, %s, %s;\n", tempReg3, tempReg3, tempReg2);
					fprintf(fp, "CMP %s, %s, {0.0, 0.0, 0.0, 0.0}, {1.0, 1.0, 1.0, 1.0};\n", tempReg2, tempReg1);
					fprintf(fp, "CMP %s, %s, {0.0, 0.0, 0.0, 0.0}, {1.0, 1.0, 1.0, 1.0};\n", tempReg3, tempReg3);
					fprintf(fp, "MUL %s, %s, %s;\n", tempReg1, tempReg2, tempReg3);
					break;
				case BINARY_NEQ:
					fprintf(fp, "TEMP %s;\n", tempReg1);
					fprintf(fp, "SUB %s, %s, %s;\n", tempReg1, tempReg2, tempReg3);
					fprintf(fp, "SUB %s, %s, %s;\n", tempReg3, tempReg3, tempReg2);
					fprintf(fp, "CMP %s, %s, {1.0, 1.0, 1.0, 1.0}, {0.0, 0.0, 0.0, 0.0};\n", tempReg2, tempReg1);
					fprintf(fp, "CMP %s, %s, {1.0, 1.0, 1.0, 1.0}, {0.0, 0.0, 0.0, 0.0};\n", tempReg2, tempReg2);
					fprintf(fp, "MUL %s, %s, %s;\n", tempReg1, tempReg2, tempReg3);
					break;
				case BINARY_LT:
					fprintf(fp, "TEMP %s;\n", tempReg1);
					fprintf(fp, "SUB %s, %s, %s;\n", tempReg1, tempReg2, tempReg3);
					fprintf(fp, "CMP %s, %s, {1.0, 1.0, 1.0, 1.0}, {0.0, 0.0, 0.0, 0.0};\n", tempReg1, tempReg1);
					break;
				case BINARY_LEQ:
					fprintf(fp, "TEMP %s;\n", tempReg1);
					fprintf(fp, "SUB %s, %s, %s;\n", tempReg1, tempReg3, tempReg2);
					fprintf(fp, "CMP %s, %s, {0.0, 0.0, 0.0, 0.0}, {1.0, 1.0, 1.0, 1.0};\n", tempReg1, tempReg1);
					break;
				case BINARY_GT:
					fprintf(fp, "TEMP %s;\n", tempReg1);
					fprintf(fp, "SUB %s, %s, %s;\n", tempReg1, tempReg3, tempReg2);
					fprintf(fp, "CMP %s, %s, {1.0, 1.0, 1.0, 1.0}, {0.0, 0.0, 0.0, 0.0};\n", tempReg1, tempReg1);
					break;
				case BINARY_GEQ:
					fprintf(fp, "TEMP %s;\n", tempReg1);
					fprintf(fp, "SUB %s, %s, %s;\n", tempReg1, tempReg2, tempReg3);
					fprintf(fp, "CMP %s, %s, {0.0, 0.0, 0.0, 0.0}, {1.0, 1.0, 1.0, 1.0};\n", tempReg1, tempReg1);
					break;
				case BINARY_PLUS:
					fprintf(fp, "TEMP %s;\n", tempReg1);
					fprintf(fp, "ADD %s, %s, %s;\n", tempReg1, tempReg2, tempReg3);
					break;
				case BINARY_MINUS:
					fprintf(fp, "TEMP %s;\n", tempReg1);
					fprintf(fp, "SUB %s, %s, %s;\n", tempReg1, tempReg2, tempReg3);
					break;
				case BINARY_TIMES:
					fprintf(fp, "TEMP %s;\n", tempReg1);
					fprintf(fp, "MUL %s, %s, %s;\n", tempReg1, tempReg2, tempReg3);
					break;
				case BINARY_DIVIDE:
					fprintf(fp, "TEMP %s;\n", tempReg1);
					fprintf(fp, "RCP %s, %s;\n", tempReg3, tempReg3);
					fprintf(fp, "MUL %s, %s, %s;\n", tempReg1, tempReg2, tempReg3);
					break;
				case BINARY_XOR: // should be power rather than xor
					fprintf(fp, "TEMP %s;\n", tempReg1);
					fprintf(fp, "POW %s, %s, %s;\n", tempReg1, tempReg2, tempReg3);
					break;
				default:
					printf("Error: genCodeRecursion() case: EXPRESSION_BINARY. You should not be here!\n");
					break;
			}
			return;
		case EXPRESSION_BOOL_VALUE:
			tempReg1 = insertExpRegList(ast);
			fprintf(fp, "TEMP %s;\n", tempReg1);
			fprintf(fp, "MOV %s, {%d.0, %d.0, %d.0, %d.0};\n", tempReg1, ast->expression_bool_value.bool_value, ast->expression_bool_value.bool_value, ast->expression_bool_value.bool_value, ast->expression_bool_value.bool_value);
			return;
		case EXPRESSION_INT_VALUE:
			tempReg1 = insertExpRegList(ast);
			fprintf(fp, "TEMP %s;\n", tempReg1);
			fprintf(fp, "MOV %s, {%d.0, %d.0, %d.0, %d.0};\n", tempReg1, ast->expression_int_value.int_value, ast->expression_int_value.int_value, ast->expression_int_value.int_value, ast->expression_int_value.int_value);
			return;
		case EXPRESSION_FLOAT_VALUE:
			tempReg1 = insertExpRegList(ast);
			fprintf(fp, "TEMP %s;\n", tempReg1);
			fprintf(fp, "MOV %s, {%f, %f, %f, %f};\n", tempReg1, ast->expression_float_value.float_value, ast->expression_float_value.float_value, ast->expression_float_value.float_value, ast->expression_float_value.float_value);
			return;
		case EXPRESSION_BRACKET:
			tempReg1 = insertExpRegList(ast);
			genCodeRecursion(ast->expression_bracket.expression);
			tempReg2 = searchExpRegList(ast->expression_bracket.expression);
			fprintf(fp, "TEMP %s;\n", tempReg1);
			fprintf(fp, "MOV %s, %s;\n", tempReg1, tempReg2);
			return;
		case EXPRESSION_VARIABLE:
			tempReg1 = insertExpRegList(ast);
			genCodeRecursion(ast->expression_variable.variable);
			tempReg2 = searchExpRegList(ast->expression_variable.variable);
			fprintf(fp, "TEMP %s;\n", tempReg1);
			fprintf(fp, "MOV %s, %s;\n", tempReg1, tempReg2);
			return;
		case VARIABLE:
			tempReg1 = insertExpRegList(ast);
			tempReg2 = searchVarRegList(ast, -1);
			fprintf(fp, "TEMP %s;\n", tempReg1);
			fprintf(fp, "MOV %s, %s;\n", tempReg1, tempReg2);
			return;
		case ARRAY:
			tempReg1 = insertExpRegList(ast);
			tempReg2 = searchVarRegList(ast, ast->array.index);
			fprintf(fp, "TEMP %s;\n", tempReg1);
			fprintf(fp, "MOV %s, %s;\n", tempReg1, tempReg2);
			return;
		case ARGUMENTS_MORE_THAN_ONE://
			return;
		case ARGUMENTS_ONLY_ONE:
			tempReg1 = insertExpRegList(ast);
			genCodeRecursion(ast->arguments_only_one.expression);
			tempReg2 = searchExpRegList(ast->arguments_only_one.expression);
			fprintf(fp, "TEMP %s;\n", tempReg1);
			fprintf(fp, "MOV %s, %s;\n", tempReg1, tempReg2);
			return;
		case ARGUMENTS_OPT:
			tempReg1 = insertExpRegList(ast);
			genCodeRecursion(ast->arguments_opt.arguments);
			tempReg2 = searchExpRegList(ast->arguments_opt.arguments);
			fprintf(fp, "TEMP %s;\n", tempReg1);
			fprintf(fp, "MOV %s, %s;\n", tempReg1, tempReg2);
			return;
		default:
			return;
  	}

	return;
}
