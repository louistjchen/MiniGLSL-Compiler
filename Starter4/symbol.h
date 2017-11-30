/*
 * Louis Chen - 1000303502
 * Juntu Chen - 1000659799
 *
 *
 *
 * */


#ifndef _SYMBOL_H
#define _SYMBOL_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ast.h"





//Construct a linked-list data structure for Symbol table.
/*

Meaning of Attribution:
'R' - Result
'A' - Attribute
'U' - Uniform
'N' - Non-constant
'C' - Constant

*/
typedef enum {
	R = 900,
	A = 901,
	U = 902,
	N = 903,
	C = 904
}attr;

typedef struct L_node{
	char* Name;
	int Attribution;
	int Value;
	int Type;
	long Line_num;
	long Scope;
	long Count;
	long Test;
	struct L_node* Next;
}L_node;


extern L_node* Head;

// Functions
void initGlobalVars();
void insertPredefVars();
void insert_node(const char* Name, int Type, long Line_num, long Scope, attr Attribution, long Count, long Test);
int is_declared(node *ast);
void symbol_table(node* ast);
void print_symbol_table(L_node* List_head);
int get_attribution(char* Name);
int is_existed(char* Name, long Scope, long Line_num);
const char* print_type(char* Name, long Scope, int Line_num);


#endif


