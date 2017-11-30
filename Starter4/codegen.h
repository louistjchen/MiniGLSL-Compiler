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


typedef struct regInfo_ {

	char *name;
	node *ast;

} regInfo;


void genCode(node *ast);
int genCodeRecursion(node *ast);


#endif	/* __CODEGEN_H__ */
