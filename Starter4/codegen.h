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

	// input information
	char varName[MAX_NAME_SIZE];
	int scopeLevel;
	int scopeIndex;

	// next regInfo pointer
	struct regInfo_ *next;

} regInfo;

void initRegList();
void insertPredefRegList(const char *name);
void insertRegList(char *varName, int scopeLevel, int scopeIndex);
char *searchRegList(char *varName, int index, int scopeLevel, int scopeIndex);
void freeRegList();

void genCode(node *ast);
int genCodeRecursion(node *ast);


#endif	/* __CODEGEN_H__ */
