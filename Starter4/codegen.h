/*
 *
 * Louis Chen - 1000303502
 * Juntu Chen - 1000659799
 *
 * This header file is for codegen.c.
 * It defines function prototypes for code generation stage.
 *
 */


#ifndef __CODEGEN_H__
#define __CODEGEN_H__ 1


#define MAX_NAME_SIZE 64

typedef struct regInfo_ {

	// output information
	char regName[MAX_NAME_SIZE];		// index = -1
	char regNameX[MAX_NAME_SIZE];		// index = 0
	char regNameY[MAX_NAME_SIZE];		// index = 1
	char regNameZ[MAX_NAME_SIZE];		// index = 2
	char regNameW[MAX_NAME_SIZE];		// index = 3

	// input information - variable/vector search
	char varName[MAX_NAME_SIZE];		// predefined & variable - variable name
						// expression - "__EXPRESSION__"
	int scopeLevel;
	int scopeIndex;

	// input information - expression search
	node *ast;				// predefined & variable - NULL
						// expression - its ast

	// next regInfo pointer
	struct regInfo_ *next;

} regInfo;

void initRegList();
void insertPredefRegList(const char *name);
char *insertVarRegList(char *varName, int scopeLevel, int scopeIndex);
char *insertExpRegList(node *ast);
char *searchVarRegList(node *ast, int index);
char *searchExpRegList(node *ast);
void freeRegList();

void genCode(node *ast);
void genCodeRecursion(node *ast);
void genCondRecursion(node *ast);

#endif	/* __CODEGEN_H__ */
