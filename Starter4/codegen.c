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


/* search varName inside regList. EXIST ? regName : NULL. Note this returns the real register for the given variable */
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

		if(strcmp(ast->variable.identifier,"env1") == 0 || strcmp(ast->variable.identifier,"env2") == 0 || strcmp(ast->variable.identifier,"env3") == 0 || strcmp(ast->variable.identifier,"gl_FragColor") == 0 || strcmp(ast->variable.identifier,"gl_TexCoord") == 0 || strcmp(ast->variable.identifier,"gl_FragCoord") == 0 || strcmp(ast->variable.identifier,"gl_Color") == 0 || strcmp(ast->variable.identifier,"gl_Secondary") == 0 || strcmp(ast->variable.identifier,"gl_Light_Half") == 0 || strcmp(ast->variable.identifier,"gl_FogFragCoord") == 0 || strcmp(ast->variable.identifier,"gl_Light_Ambient") == 0 || strcmp(ast->variable.identifier,"gl_Material_Shininess") == 0 || strcmp(ast->variable.identifier,"gl_FragDepth") == 0)

			while(Cur != NULL) {
				if(strcmp(ast->variable.identifier, Cur->Name) == 0) {
					Target = Cur;
					goto foundSymbolEntry;
				}
				else
					Cur = Cur->Next;
			}

		Cur = Head;
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

		if(strcmp(ast->array.identifier,"env1") == 0 || strcmp(ast->array.identifier,"env2") == 0 || strcmp(ast->array.identifier,"env3") == 0 || strcmp(ast->array.identifier,"gl_FragColor") == 0 || strcmp(ast->array.identifier,"gl_TexCoord") == 0 || strcmp(ast->array.identifier,"gl_FragCoord") == 0 || strcmp(ast->array.identifier,"gl_Color") == 0 || strcmp(ast->array.identifier,"gl_Secondary") == 0 || strcmp(ast->array.identifier,"gl_Light_Half") == 0 || strcmp(ast->array.identifier,"gl_FogFragCoord") == 0 || strcmp(ast->array.identifier,"gl_Light_Ambient") == 0 || strcmp(ast->array.identifier,"gl_Material_Shininess") == 0 || strcmp(ast->array.identifier,"gl_FragDepth") == 0)

			while(Cur != NULL) {
				if(strcmp(ast->array.identifier, Cur->Name) == 0) {
					Target = Cur;
					goto foundSymbolEntry;
				}
				else
					Cur = Cur->Next;
			}

		Cur = Head;
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

foundSymbolEntry:
	
	if(Target == NULL) {
		printf("Searching the regInfo that has correct scopeLevel/scopeIndex as the input variable. Target == NULL. You should not be here!\n");
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
		printf("Searching the regInfo that has correct scopeLevel/scopeIndex as the input variable. ast->kind != VARIABLE|ARRAY. You should not be here!\n");
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
		printf("Searching the regInfo that has correct scopeLevel/scopeIndex as the input variable. found == NULL. You should not be here!\n");
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


/* search expression given its ast inside regList. EXIST ? regName : NULL. Note this returns the wrapper register of the given expression. If this expression were a variable expression, then it will return the real register for the variable. If not, it will return the wrapper register. */
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


/* Toplevel function for code generation */
void genCode(node *ast) {


	// copy file pointer; no need to close file as compiler467.c does it
	fp = outputFile;
	if(fp == NULL) {
		printf("Cannot open/create \"frag.txt\". Exit.\n");
		return;
	}

	// print ARB assembly header
	fprintf(fp, "!!ARBfp1.0\n\n");

	// traverse AST and generate code for condition registers
	// need to assign the root (program) condition register to true
	// allocate a "condBuf" register for future condReg buffering
	fprintf(fp, "#generating condition registers...\n");
	sprintf(ast->condReg, "cond_%d_%d", ast->scopeLevel, ast->scopeIndex);
	fprintf(fp, "TEMP %s;\n", ast->condReg);
	fprintf(fp, "TEMP condBuf;\n");
	fprintf(fp, "MOV %s, {1.0, 1.0, 1.0, 1.0};\n", ast->condReg);
	genCondRecursion(ast);

	// traverse AST and generate ARB assembly code
	fprintf(fp, "\n#generating ARB assembly code...\n");
	genCodeRecursion(ast);

	// print ARB assembly footer
	fprintf(fp, "\nEND\n");
	
	return;
}


/* Recursion function for code generation (post-order traversal) */
void genCodeRecursion(node *ast) {

	// declare some temp variables to store return values from recursions
	char *tempReg1;
	char *tempReg2;
	char *tempReg3;

	// declare some temp variables for argument list buffer
	int i, numArgs;
	node *tempAst;
	node **tempArgs;
	char **tempRegs;

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
			if(ast->declaration_assign.expression->kind == EXPRESSION_VARIABLE) {
				if(ast->declaration_assign.expression->expression_variable.variable->kind == VARIABLE)
					tempReg2 = searchVarRegList(ast->declaration_assign.expression->expression_variable.variable, -1);
				else if(ast->declaration_assign.expression->expression_variable.variable->kind == ARRAY)
					tempReg2 = searchVarRegList(ast->declaration_assign.expression->expression_variable.variable, ast->declaration_assign.expression->expression_variable.variable->array.index);
				else {
					printf("Error: genCodeRecursion() case: DECLARATION_ASSIGN. You should not be here!\n");
					return;
				}
			}
			else {
				genCodeRecursion(ast->declaration_assign.expression);
				tempReg2 = searchExpRegList(ast->declaration_assign.expression);
			}

			// Here we use current AST's condReg (condition register) to determine whether to assign or not
			// if current scope is outermost, we simply use MOV
			// else, we use CMP for assignment
			fprintf(fp, "TEMP %s;\n", tempReg1);
			if(ast->scopeLevel == 1 && ast->scopeIndex == 1)
				fprintf(fp, "MOV %s, %s;\n", tempReg1, tempReg2);
			else {
				fprintf(fp, "SUB condBuf, %s, 1.0;\n", ast->condReg);
				fprintf(fp, "CMP %s, condBuf, %s, %s;\n", tempReg1, tempReg1, tempReg2);
			}
			return;
		case DECLARATION_ASSIGN_CONST:
			tempReg1 = insertVarRegList(ast->declaration_assign_const.identifier, ast->scopeLevel, ast->scopeIndex);

			if(ast->declaration_assign_const.expression->kind == EXPRESSION_VARIABLE) {
				if(ast->declaration_assign_const.expression->expression_variable.variable->kind == VARIABLE)
					tempReg2 = searchVarRegList(ast->declaration_assign_const.expression->expression_variable.variable, -1);
				else if(ast->declaration_assign_const.expression->expression_variable.variable->kind == ARRAY)
					tempReg2 = searchVarRegList(ast->declaration_assign_const.expression->expression_variable.variable, ast->declaration_assign_const.expression->expression_variable.variable->array.index);
				else {
					printf("Error: genCodeRecursion() case: DECLARATION_ASSIGN_CONST. You should not be here!\n");
					return;
				}
			}
			else {
				genCodeRecursion(ast->declaration_assign_const.expression);
				tempReg2 = searchExpRegList(ast->declaration_assign_const.expression);
			}
			// due to our structure, need to use TEMP instead of PARAM for const
			// however we have checked const during semantic check stage
			//fprintf(fp, "PARAM %s = %s;\n", tempReg1, tempReg2);
			
			// Here we use current AST's condReg (condition register) to determine whether to assign or not
			// if current scope is outermost, we simply use MOV
			// else, we use CMP for assignment
			fprintf(fp, "TEMP %s;\n", tempReg1);
			if(ast->scopeLevel == 1 && ast->scopeIndex == 1)
				fprintf(fp, "MOV %s, %s;\n", tempReg1, tempReg2);
			else {
				fprintf(fp, "SUB condBuf, %s, 1.0;\n", ast->condReg);
				fprintf(fp, "CMP %s, condBuf, %s, %s;\n", tempReg1, tempReg1, tempReg2);
			}
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

			if(ast->statement_assign.expression->kind == EXPRESSION_VARIABLE) {
				if(ast->statement_assign.expression->expression_variable.variable->kind == VARIABLE)
					tempReg2 = searchVarRegList(ast->statement_assign.expression->expression_variable.variable, -1);
				else if(ast->statement_assign.expression->expression_variable.variable->kind == ARRAY)
					tempReg2 = searchVarRegList(ast->statement_assign.expression->expression_variable.variable, ast->statement_assign.expression->expression_variable.variable->array.index);
				else {
					printf("Error: genCodeRecursion() case: STATEMENT_ASSIGN/VARIABLE. You should not be here!\n");
					return;
				}
			}
			else {
				genCodeRecursion(ast->statement_assign.expression);
				tempReg2 = searchExpRegList(ast->statement_assign.expression);
			}

			
			// Here we use current AST's condReg (condition register) to determine whether to assign or not
			// if current scope is outermost && current AST's parent is not a conditional statement, we simply use MOV. Need to know parent node kind to avoid assignment in ELSE without parentheses
			// else, we use CMP for assignment
			if(ast->scopeLevel == 1 && ast->scopeIndex == 1 && ast->parent->kind != STATEMENT_IF_ELSE && ast->parent->kind != STATEMENT_IF)
				fprintf(fp, "MOV %s, %s;\n", tempReg1, tempReg2);
			else {
				fprintf(fp, "SUB condBuf, %s, 1.0;\n", ast->condReg);
				fprintf(fp, "CMP %s, condBuf, %s, %s;\n", tempReg1, tempReg1, tempReg2);
			}
			return;
		case STATEMENT_IF_ELSE:
			genCodeRecursion(ast->statement_if_else.statement_valid);
			genCodeRecursion(ast->statement_if_else.statement_invalid);
			return;
		case STATEMENT_IF:
			genCodeRecursion(ast->statement_if.statement_valid);
			return;
		case TYPE:
			printf("Error: genCodeRecursion() case: TYPE. You should not be here!\n");
			return;
		case EXPRESSION_TYPE:
			// input arguments at most 4 element
			// we bruteforce to get at most 4 ast pointers that point to all arguments
			tempReg1 = insertExpRegList(ast);

			tempAst = ast->expression_type.arguments;
			numArgs = numArguments(tempAst);
			tempArgs = (node **)malloc(sizeof(node *) * numArgs);
			tempRegs = (char **)malloc(sizeof(char *) * numArgs);

			i = 0;
			while(i < numArgs) {
			
				if(tempAst->kind == ARGUMENTS_ONLY_ONE) {
					if(i != numArgs-1) {
						printf("Error: genCodeRecursion() case: EXPRESSION_TYPE/ARGUMENTS_ONLY_ONE. You should not be here!\n");
						free(tempArgs);
						free(tempRegs);
						return;
					}
					tempArgs[numArgs-i-1] = tempAst->arguments_only_one.expression;
					i++;
				}
				else if(tempAst->kind == ARGUMENTS_MORE_THAN_ONE) {
					if(i < 0 || i > numArgs-2) {
						printf("Error: genCodeRecursion() case: EXPRESSION_TYPE/ARGUMENTS_MORE_THAN_ONE. You should not be here!\n");
						free(tempArgs);
						free(tempRegs);
						return;
					}
					tempArgs[numArgs-i-1] = tempAst->arguments_more_than_one.expression;
					tempAst = tempAst->arguments_more_than_one.arguments_more_than_one;
					i++;
				}

				else if(tempAst->kind == ARGUMENTS_OPT) {
					if(i != 0) {
						printf("Error: genCodeRecursion() case: EXPRESSION_TYPE/ARGUMENTS_OPT. You should not be here!\n");
						free(tempArgs);
						free(tempRegs);
						return;
					}
					tempAst = tempAst->arguments_opt.arguments;
				}
			}

			for(i = numArgs-1; i >= 0; i--) {
				genCodeRecursion(tempArgs[i]);
				tempRegs[i] = searchExpRegList(tempArgs[i]);
			}


			fprintf(fp, "TEMP %s;\n", tempReg1);			
			for(i = 0; i < numArgs; i++) {
				switch(i) {
					case 0:
						fprintf(fp, "MOV %s.x, %s;\n", tempReg1, tempRegs[i]);
						break;
					case 1:
						fprintf(fp, "MOV %s.y, %s;\n", tempReg1, tempRegs[i]);
						break;
					case 2:
						fprintf(fp, "MOV %s.z, %s;\n", tempReg1, tempRegs[i]);
						break;
					case 3:
						fprintf(fp, "MOV %s.w, %s;\n", tempReg1, tempRegs[i]);
						break;
					default:
						printf("Error: genCodeRecursion() case: EXPRESSION_TYPE/MOVxyzw. You should not be here!\n");
						break;
				}
			}

			free(tempArgs);
			free(tempRegs);
			return;
		case EXPRESSION_FUNC:
			// input arguments at most 2 elements, can be either scalar or vector
			// we bruteforce to get at most 2 ast pointers that point to all arguments
			tempReg1 = insertExpRegList(ast);

			tempAst = ast->expression_type.arguments;
			numArgs = numArguments(tempAst);
			tempArgs = (node **)malloc(sizeof(node *) * numArgs);
			tempRegs = (char **)malloc(sizeof(char *) * numArgs);

			i = 0;
			while(i < numArgs) {
			
				if(tempAst->kind == ARGUMENTS_ONLY_ONE) {
					if(i != numArgs-1) {
						printf("Error: genCodeRecursion() case: EXPRESSION_FUNC/ARGUMENTS_ONLY_ONE. You should not be here!\n");
						free(tempArgs);
						free(tempRegs);
						return;
					}
					tempArgs[numArgs-i-1] = tempAst->arguments_only_one.expression;
					i++;
				}
				else if(tempAst->kind == ARGUMENTS_MORE_THAN_ONE) {
					if(i < 0 || i > numArgs-2) {
						printf("Error: genCodeRecursion() case: EXPRESSION_FUNC/ARGUMENTS_MORE_THAN_ONE. You should not be here!\n");
						free(tempArgs);
						free(tempRegs);
						return;
					}
					tempArgs[numArgs-i-1] = tempAst->arguments_more_than_one.expression;
					tempAst = tempAst->arguments_more_than_one.arguments_more_than_one;
					i++;
				}

				else if(tempAst->kind == ARGUMENTS_OPT) {
					if(i != 0) {
						printf("Error: genCodeRecursion() case: EXPRESSION_FUNC/ARGUMENTS_OPT. You should not be here!\n");
						free(tempArgs);
						free(tempRegs);
						return;
					}
					tempAst = tempAst->arguments_opt.arguments;
				}
			}

			for(i = numArgs-1; i >= 0; i--) {

				if(tempArgs[i]->kind == EXPRESSION_VARIABLE) {
					if(tempArgs[i]->expression_variable.variable->kind == VARIABLE)
						tempRegs[i] = searchVarRegList(tempArgs[i]->expression_variable.variable, -1);
					else if(tempArgs[i]->expression_variable.variable->kind == ARRAY)
						tempRegs[i] = searchVarRegList(tempArgs[i]->expression_variable.variable, tempArgs[i]->expression_variable.variable->array.index);
					else {
						printf("Error: genCodeRecursion() case: EXPRESSION_FUNC/EXPRESSION_VARIABLE. You should not be here!\n");
						free(tempArgs);
						free(tempRegs);
						return;
					}
				}
				else {
					genCodeRecursion(tempArgs[i]);
					tempRegs[i] = searchExpRegList(tempArgs[i]);
				}
			}


			switch(ast->expression_func.function) {
			
				case 0: // DP3
					if(numArgs != 2) {
						printf("Error: genCodeRecursion() case: EXPRESSION_FUNC/DP3. You should not be here!\n");
						break;
					}
					fprintf(fp, "TEMP %s;\n", tempReg1);
					fprintf(fp, "DP3 %s, %s, %s;\n", tempReg1, tempRegs[0], tempRegs[1]);
					break;
				case 1: // LIT
					if(numArgs != 1) {
						printf("Error: genCodeRecursion() case: EXPRESSION_FUNC/LIT. You should not be here!\n");
						break;
					}
					fprintf(fp, "TEMP %s;\n", tempReg1);
					fprintf(fp, "LIT %s, %s;\n", tempReg1, tempRegs[0]);
					break;
				case 2: // RSQ
					if(numArgs != 1) {
						printf("Error: genCodeRecursion() case: EXPRESSION_FUNC/RSQ. You should not be here!\n");
						break;
					}
					fprintf(fp, "TEMP %s;\n", tempReg1);
					fprintf(fp, "RSQ %s, %s;\n", tempReg1, tempRegs[0]);
					break;
				default:
					printf("Error: genCodeRecursion() case: EXPRESSION_FUNC/default. You should not be here!\n");
					break;
			}

			free(tempArgs);
			free(tempRegs);
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
			// Here we determine the logical operations based on available ARB opcodes
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
			// we create a wrapper expression register that stores the bool value, in float type
			tempReg1 = insertExpRegList(ast);
			fprintf(fp, "TEMP %s;\n", tempReg1);
			fprintf(fp, "MOV %s, {%d.0, %d.0, %d.0, %d.0};\n", tempReg1, ast->expression_bool_value.bool_value, ast->expression_bool_value.bool_value, ast->expression_bool_value.bool_value, ast->expression_bool_value.bool_value);
			return;
		case EXPRESSION_INT_VALUE:
			// we create a wrapper expression register that stores the int value, in float type
			tempReg1 = insertExpRegList(ast);
			fprintf(fp, "TEMP %s;\n", tempReg1);
			fprintf(fp, "MOV %s, {%d.0, %d.0, %d.0, %d.0};\n", tempReg1, ast->expression_int_value.int_value, ast->expression_int_value.int_value, ast->expression_int_value.int_value, ast->expression_int_value.int_value);
			return;
		case EXPRESSION_FLOAT_VALUE:
			// we create a wrapper expression register that stores the float value
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
			// we create a wrapper register and assign it to the corresponding variable's register
			tempReg1 = insertExpRegList(ast);
			// comment this out to avoid extra wrapper registers
			// therefore we should never recursively go into VARIABLE/ARRAY
			//genCodeRecursion(ast->expression_variable.variable);
			//tempReg2 = searchExpRegList(ast->expression_variable.variable);
			if(ast->expression_variable.variable->kind == VARIABLE)
				tempReg2 = searchVarRegList(ast->expression_variable.variable, -1);
			else if(ast->expression_variable.variable->kind == ARRAY)
				tempReg2 = searchVarRegList(ast->expression_variable.variable, ast->expression_variable.variable->array.index);
			fprintf(fp, "TEMP %s;\n", tempReg1);
			fprintf(fp, "MOV %s, %s;\n", tempReg1, tempReg2);
			return;
		case VARIABLE:
			printf("Error: genCodeRecursion() case: VARIABLE. You should not be here!\n");
			//tempReg1 = insertExpRegList(ast);
			//tempReg2 = searchVarRegList(ast, -1);
			//fprintf(fp, "TEMP %s;\n", tempReg1);
			//fprintf(fp, "MOV %s, %s;\n", tempReg1, tempReg2);
			return;
		case ARRAY:
			printf("Error: genCodeRecursion() case: ARRAY. You should not be here!\n");
			//tempReg1 = insertExpRegList(ast);
			//tempReg2 = searchVarRegList(ast, ast->array.index);
			//fprintf(fp, "TEMP %s;\n", tempReg1);
			//fprintf(fp, "MOV %s, %s;\n", tempReg1, tempReg2);
			return;
		case ARGUMENTS_MORE_THAN_ONE:
			printf("Error: genCodeRecursion() case: ARGUMENTS_MORE_THAN_ONE. You should not be here!\n");
			return;
		case ARGUMENTS_ONLY_ONE:
			printf("Error: genCodeRecursion() case: ARGUMENTS_ONLY_ONE. You should not be here!\n");
			return;
		case ARGUMENTS_OPT:
			printf("Error: genCodeRecursion() case: ARGUMENTS_OPT. You should not be here!\n");
			return;
		default:
			printf("Error: genCodeRecursion() case: default. You should not be here!\n");
			return;
  	}

	return;
}


/* Recursion function for condition register generation */
void genCondRecursion(node *ast) {

	char *tempReg1;

	// return when reaching empty node
	if(ast == NULL)
		return;

  	switch(ast->kind) {

		case PROGRAM:
			// assign condReg to child node
			if(ast->program.scope != NULL)
				sprintf(ast->program.scope->condReg, "%s", ast->condReg);
			genCondRecursion(ast->program.scope);
			return;
		case SCOPE:
			// assign condReg to child nodes
			if(ast->scope.declarations != NULL)
				sprintf(ast->scope.declarations->condReg, "%s", ast->condReg);
			if(ast->scope.statements != NULL)
				sprintf(ast->scope.statements->condReg, "%s", ast->condReg);
			genCondRecursion(ast->scope.declarations);
			genCondRecursion(ast->scope.statements);
			return;
		case DECLARATIONS:
			// assign condReg to child nodes
			if(ast->declarations.declarations != NULL)
				sprintf(ast->declarations.declarations->condReg, "%s", ast->condReg);
			if(ast->declarations.declaration != NULL)
				sprintf(ast->declarations.declaration->condReg, "%s", ast->condReg);
			genCondRecursion(ast->declarations.declarations);
			return;
		case STATEMENTS:
			// assign condReg to child nodes
			if(ast->statements.statements != NULL)
				sprintf(ast->statements.statements->condReg, "%s", ast->condReg);
			if(ast->statements.statement != NULL)
				sprintf(ast->statements.statement->condReg, "%s", ast->condReg);
			genCondRecursion(ast->statements.statements);
			// only run recursion if child is an if/if_else statement
			if(ast->statements.statement->kind != STATEMENT_ASSIGN)
				genCondRecursion(ast->statements.statement);
			return;
		case DECLARATION:
			printf("Error: genCondRecursion() case DECLARATION. You should not be here!\n");
			return;
		case DECLARATION_ASSIGN:
			printf("Error: genCondRecursion() case DECLARATION_ASSIGN. You should not be here!\n");
			return;
		case DECLARATION_ASSIGN_CONST:
			printf("Error: genCondRecursion() case DECLARATION_ASSIGN_CONST. You should not be here!\n");
			return;
		case STATEMENT_ASSIGN:
			printf("Error: genCondRecursion() case STATEMENT_ASSIGN. You should not be here!\n");
			return;
		case STATEMENT_IF_ELSE:
			// 1) Run genCodeRecursion on expression to fetch its wrapper register
			genCodeRecursion(ast->statement_if_else.expression);
			tempReg1 = searchExpRegList(ast->statement_if_else.expression);

			// 2) For IF, "AND" expression register with current AST's condReg and assign to statement_valid's condReg
			if(ast->statement_if_else.statement_valid != NULL) {
				sprintf(ast->statement_if_else.statement_valid->condReg, "cond_%d_%d", ast->statement_if_else.statement_valid->scopeLevel, ast->statement_if_else.statement_valid->scopeIndex);
				fprintf(fp, "TEMP %s;\n", ast->statement_if_else.statement_valid->condReg);
				fprintf(fp, "MUL %s, %s, %s;\n", ast->statement_if_else.statement_valid->condReg, ast->condReg, tempReg1);
			}

			// 3) For ELSE, "NOT" expression register, "AND" the result with current AST's condReg and assign to statement_invalid's condReg
			if(ast->statement_if_else.statement_invalid != NULL) {
				sprintf(ast->statement_if_else.statement_invalid->condReg, "cond_%d_%d", ast->statement_if_else.statement_invalid->scopeLevel, ast->statement_if_else.statement_invalid->scopeIndex);
				fprintf(fp, "TEMP %s;\n", ast->statement_if_else.statement_invalid->condReg);
				fprintf(fp, "SUB %s, 1.0, %s;\n", tempReg1, tempReg1);
				fprintf(fp, "MUL %s, %s, %s;\n", ast->statement_if_else.statement_invalid->condReg, ast->condReg, tempReg1);
			}

			genCondRecursion(ast->statement_if_else.statement_valid);
			genCondRecursion(ast->statement_if_else.statement_invalid);
			return;
		case STATEMENT_IF:
			// 1) Run genCodeRecursion on expression to fetch its wrapper register
			genCodeRecursion(ast->statement_if.expression);
			tempReg1 = searchExpRegList(ast->statement_if.expression);

			// 2) For IF, "AND" expression register with current AST's condReg and assign to statement_valid's condReg
			if(ast->statement_if.statement_valid != NULL) {
				sprintf(ast->statement_if.statement_valid->condReg, "cond_%d_%d", ast->statement_if.statement_valid->scopeLevel, ast->statement_if.statement_valid->scopeIndex);
				fprintf(fp, "TEMP %s;\n", ast->statement_if.statement_valid->condReg);
				fprintf(fp, "MUL %s, %s, %s;\n", ast->statement_if.statement_valid->condReg, ast->condReg, tempReg1);
			}

			genCondRecursion(ast->statement_if.statement_valid);
			return;
		case TYPE:
			printf("Error: genCondRecursion() case TYPE. You should not be here!\n");
			return;
		case EXPRESSION_TYPE:
			printf("Error: genCondRecursion() case EXPRESSION_TYPE. You should not be here!\n");
			return;
		case EXPRESSION_FUNC:
			printf("Error: genCondRecursion() case EXPRESSION_FUNC. You should not be here!\n");
			return;
		case EXPRESSION_UNARY:
			printf("Error: genCondRecursion() case EXPRESSION_UNARY. You should not be here!\n");
			return;
		case EXPRESSION_BINARY:
			printf("Error: genCondRecursion() case EXPRESSION_BINARY. You should not be here!\n");
			return;
		case EXPRESSION_BOOL_VALUE:
			printf("Error: genCondRecursion() case EXPRESSION_BOOL_VALUE. You should not be here!\n");
			return;
		case EXPRESSION_INT_VALUE:
			printf("Error: genCondRecursion() case EXPRESSION_INT_VALUE. You should not be here!\n");
			return;
		case EXPRESSION_FLOAT_VALUE:
			printf("Error: genCondRecursion() case EXPRESSION_FLOAT_VALUE. You should not be here!\n");
			return;
		case EXPRESSION_BRACKET:
			printf("Error: genCondRecursion() case EXPRESSION_BRACKET. You should not be here!\n");
			return;
		case EXPRESSION_VARIABLE:
			printf("Error: genCondRecursion() case EXPRESSION_VARIABLE. You should not be here!\n");
			return;
		case VARIABLE:
			printf("Error: genCondRecursion() case VARIABLE. You should not be here!\n");
			return;
		case ARRAY:
			printf("Error: genCondRecursion() case ARRAY. You should not be here!\n");
			return;
		case ARGUMENTS_MORE_THAN_ONE:
			printf("Error: genCondRecursion() case ARGUMENTS_MORE_THAN_ONE. You should not be here!\n");
			return;
		case ARGUMENTS_ONLY_ONE:
			printf("Error: genCondRecursion() case ARGUMENTS_ONLY_ONE. You should not be here!\n");
			return;
		case ARGUMENTS_OPT:
			printf("Error: genCondRecursion() case ARGUMENTS_OPT. You should not be here!\n");
			return;
		default:
			printf("Error: genCondRecursion() case default. You should not be here!\n");
			return;
  	}

	return;
}
