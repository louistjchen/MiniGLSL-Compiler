/*
 * * Louis Chen - 1000303502
 * Juntu Chen - 1000659799

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
	
	struct L_node* Next;
}L_node;


extern L_node* Head;
extern long Scope_num;



// Functions
void insert_node(char* Name, int Type, long Line_num, long Scope, attr Attribution, long Count);
int get_nodeType(char* Name, long Scope);
int is_declared(char* Name, long Scope, long Line_num);
void symbol_table(node* ast);
void print_symbol_table(L_node* List_head);
int get_attribution(char* Name);
int is_existed(char* Name, long Scope, long Line_num);


#endif


