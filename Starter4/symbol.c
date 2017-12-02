/*
 * Louis Chen - 1000303502
 * Juntu Chen - 1000659799
 *
 *
 *
 * */


#include "common.h"
#include "symbol.h"

// Global variables
L_node* Head = NULL;

// local variables
long Dummy_count = 0;

// initialize global variables
void initGlobalVars() {

	int i;
	for(i = 0; i < 255; i++) {
		configScopeCount[i] = 0;
		global_dummy_count[i] = 0;
		Test_count[i] = 0;
		printScopeDummy[i] = 0;
	}
	return;
}


// wrapper function for inserting all predefined variables
void insertPredefVars() {

	insert_node("env1", VEC4, 0, 0, U, 0, 0);
	insert_node("env2", VEC4, 0, 0, U, 0, 0);
	insert_node("env3", VEC4, 0, 0, U, 0, 0);
	insert_node("gl_FragColor", VEC4, 0, 0, R, 0, 0);
	insert_node("gl_TexCoord", VEC4, 0, 0, A, 0, 0);
	insert_node("gl_FragCoord", VEC4, 0, 0, R, 0, 0);
	insert_node("gl_Color", VEC4, 0, 0, A, 0, 0);
	insert_node("gl_Secondary", VEC4, 0, 0, A, 0, 0);
	insert_node("gl_Light_Half", VEC4, 0, 0, U, 0, 0);
	insert_node("gl_FogFragCoord", VEC4, 0, 0, A, 0, 0);
	insert_node("gl_Light_Ambient", VEC4, 0, 0, U, 0, 0);
	insert_node("gl_Material_Shininess", VEC4, 0, 0, U, 0, 0);
	insert_node("gl_FragDepth", BOOL, 0, 0, R, 0, 0);

	return;
}


// Linked-list functions
void insert_node(const char* Name, int Type, long Line_num, long Scope, attr Attribution, long Count, long Test){
	// Creat a new list node for the variable, since we don't want to mess up the symbol table with AST tree nodes.
	L_node* Cur;
	Cur = (struct L_node*)malloc(sizeof (L_node));
	Cur->Name = (char*)malloc(sizeof(char) * (strlen(Name) + 1));
	
	// Store infomation of the node.
	Cur->Type = Type;
	Cur->Line_num = Line_num;
	Cur->Scope = Scope;
	Cur->Attribution = (int)Attribution;
	Cur->Count = Dummy_count;
	Cur->Test = Test;
	strcpy(Cur->Name, Name);
	
	// Like a stack, new nodes are pushed to the front of the list.
	if(Head == NULL){
		Cur->Next = NULL;
		Head = Cur;
	}	
	else{
		Cur->Next = Head;
		Head = Cur;
	}
}

const char* print_type(char* Name, long Scope, int Line_num){
	L_node* Cur;
	Cur = Head;
	int var_type = 0;
	while(Cur != NULL){
		if(strcmp(Cur->Name, Name) == 0 && (Cur->Scope <= Scope) && (Cur->Line_num <= Line_num)){
			// At parent scope
			if(Cur->Scope < Scope){
				var_type = Cur->Type;
			}
			// At current scope
			else if(Cur->Test == printScopeDummy[Scope]){
				var_type = Cur->Type;
				break;
			}
		}
		Cur = Cur->Next;
	}
	if(var_type == 0){
		printf("Error!\n");
		return "ERROR";
	}
	switch (var_type){
		case 100:
			return "INT";
		case 101:
			return "IVEC2";
		case 102:
			return "IVEC3";
		case 103:
			return "IVEC4";
		case 104:
			return "FLOAT";
		case 105:
			return "VEC2";
		case 106:
			return "VEC3";
		case 107:
			return "VEC4";
		case 108:
			return "BOOL";
		case 109:
			return "BVEC2";
		case 110:
			return "BVEC3";
		case 111:
			return "BVEC4";
		case 112:
			return "FUNCTION";
		default:
			return "ERROR";
	}
	return "ERROR";
}


int get_attribution(char* Name){
	L_node* Cur;
	
	Cur = Head;
	while(Cur != NULL){
		if (strcmp(Cur->Name, Name) != 0)
			Cur = Cur->Next;
		else
			return Cur->Attribution;
	}
	printf("Such a Name doesn't exists in the symbol table.\n");
	return -1;
}


// Check if the ID is declared in the same Scope or parent scopes.
int is_declared(node* ast){

	L_node* Cur = NULL;
	node* Temp = NULL;
	L_node* Target = NULL;
	
	Cur = Head;

	if(ast->kind == VARIABLE) {
		
		if(strcmp(ast->variable.identifier,"env1") == 0 || strcmp(ast->variable.identifier,"env2") == 0 || strcmp(ast->variable.identifier,"env3") == 0 || strcmp(ast->variable.identifier,"gl_FragColor") == 0 || strcmp(ast->variable.identifier,"gl_TexCoord") == 0 || strcmp(ast->variable.identifier,"gl_FragCoord") == 0 || strcmp(ast->variable.identifier,"gl_Color") == 0 || strcmp(ast->variable.identifier,"gl_Secondary") == 0 || strcmp(ast->variable.identifier,"gl_Light_Half") == 0 || strcmp(ast->variable.identifier,"gl_FogFragCoord") == 0 || strcmp(ast->variable.identifier,"gl_Light_Ambient") == 0 || strcmp(ast->variable.identifier,"gl_Material_Shininess") == 0)
			return 107;	
		else if(strcmp(ast->variable.identifier,"gl_FragDepth") == 0)
			return 108;

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
					return -1;
				}	
			}
			if(Target != NULL)
				break;
			Cur = Cur->Next;
		}
	}
	else if(ast->kind == ARRAY) {

		if(strcmp(ast->array.identifier,"env1") == 0 || strcmp(ast->array.identifier,"env2") == 0 || strcmp(ast->array.identifier,"env3") == 0 || strcmp(ast->array.identifier,"gl_FragColor") == 0 || strcmp(ast->array.identifier,"gl_TexCoord") == 0 || strcmp(ast->array.identifier,"gl_FragCoord") == 0 || strcmp(ast->array.identifier,"gl_Color") == 0 || strcmp(ast->array.identifier,"gl_Secondary") == 0 || strcmp(ast->array.identifier,"gl_Light_Half") == 0 || strcmp(ast->array.identifier,"gl_FogFragCoord") == 0 || strcmp(ast->array.identifier,"gl_Light_Ambient") == 0 || strcmp(ast->array.identifier,"gl_Material_Shininess") == 0)
			return 107;	
		else if(strcmp(ast->array.identifier,"gl_FragDepth") == 0)
			return 108;

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
					return -1;
				}	
			}
			if(Target != NULL)
				break;
			Cur = Cur->Next;
		}
	}
	else {	
		printf("AST node is not a variable/array node.\n");
		return -1;
	}
	
	
	if(Target == NULL)
		return -1;
	else
		return Target->Type;
		
}








// This one is for checking declaration for same ID in the same scope. If declaration is not in the same scope, it is valid (shadowing).
int is_existed(char* Name, long Scope, long Line_num){
	L_node* Cur;
	L_node* Temp;
	
	Cur = Head;
	
	if(strcmp(Name,"env1") == 0 || strcmp(Name,"env2") == 0 || strcmp(Name,"env3") == 0 || strcmp(Name,"gl_FragColor") == 0 || strcmp(Name,"gl_TexCoord") == 0 || strcmp(Name,"gl_FragCoord") == 0 || strcmp(Name,"gl_Color") == 0 || strcmp(Name,"gl_Secondary") == 0 || strcmp(Name,"gl_Light_Half") == 0 || strcmp(Name,"gl_FogFragCoord") == 0 || strcmp(Name,"gl_Light_Ambient") == 0 || strcmp(Name,"gl_Material_Shininess") == 0)
			return 107;	
		else if(strcmp(Name,"gl_FragDepth") == 0)
			return 108;

	while(Cur != NULL){
		if(strcmp(Cur->Name, Name) == 0 && (Cur->Scope == 0))
			return Cur->Attribution;
		Cur = Cur->Next;
	}
	
	Temp = NULL;
	Cur = Head;
	while(Cur != NULL){
		if(strcmp(Cur->Name, Name) == 0 && (Cur->Scope == Scope))
			if(Cur->Line_num == Line_num){
				 Temp = Cur;
				 break;
			}
		Cur = Cur->Next;
	}
	
	if(Temp == NULL)
		return -1;
	else{
		Cur = Head;
		while(Cur != NULL){
			if(strcmp(Cur->Name, Temp->Name) == 0 && (Cur->Scope == Temp->Scope)){
				if((Cur->Line_num < Temp->Line_num) && (Cur->Count == Temp->Count))
					 return Cur->Type;
			}
			Cur = Cur->Next;
		}
		return -1;
	}
}

void print_symbol_table(L_node* List_head){
	L_node* Temp;
	Temp = List_head;
	int counter = 0;
	while(Temp != NULL){
		printf("Node %d - Name: %s, Attribution: %d, Value: %d, Type: %d, Ln: %ld, ScopeLevel: %ld , ScopeIndex: %ld\n", counter, Temp->Name, Temp->Attribution, Temp->Value, Temp->Type, Temp->Line_num, Temp->Scope, Temp->Test);
		counter ++;
		Temp = Temp->Next;
	}
}

void symbol_table(node* ast){

	if (ast == NULL)
		return;
	
	switch(ast->kind){
		case PROGRAM:
			//Every time get into a new {}, the scope number should increase.
			Scope_num ++;
			Dummy_count ++;
			Test_count[Scope_num]++;
			symbol_table(ast->program.scope);
			Scope_num --;
			break;
			
		case SCOPE:
			symbol_table(ast->scope.declarations);
			symbol_table(ast->scope.statements);
			break;
					
		case DECLARATIONS:
			symbol_table(ast->declarations.declarations);
			symbol_table(ast->declarations.declaration);
			break;
		
		case STATEMENTS:
			symbol_table(ast->statements.statements);
			symbol_table(ast->statements.statement);
			break;
		
		case DECLARATION:
			symbol_table(ast->declaration.type);
			// Identifier is a Terminal and the name of a var, so we insert it into the symbol table
			insert_node(ast->declaration.identifier, ast->declaration.type->type.type, 
			ast->declaration.ln, Scope_num, N, Dummy_count, Test_count[Scope_num]);
			break;
		
		case DECLARATION_ASSIGN:
			symbol_table(ast->declaration_assign.type);
			
			insert_node(ast->declaration_assign.identifier, ast->declaration_assign.type->type.type,
			ast->declaration_assign.ln, Scope_num, N, Dummy_count, Test_count[Scope_num]);
			
			symbol_table(ast->declaration_assign.expression);
			break;
			
		case DECLARATION_ASSIGN_CONST:
			symbol_table(ast->declaration_assign.type);
			
			insert_node(ast->declaration_assign_const.identifier, 
			ast->declaration_assign_const.type->type.type,
			ast->declaration_assign_const.ln, Scope_num, C, Dummy_count, Test_count[Scope_num]);
			
			symbol_table(ast->declaration_assign.expression);
			break;
		
		case STATEMENT_ASSIGN:
			symbol_table(ast->statement_assign.variable);
			symbol_table(ast->statement_assign.expression);
			break;
		
		case STATEMENT_IF_ELSE:
			symbol_table(ast->statement_if_else.expression);
			symbol_table(ast->statement_if_else.statement_valid);
			symbol_table(ast->statement_if_else.statement_invalid);
			break;
		
		case STATEMENT_IF:
			symbol_table(ast->statement_if.expression);
			symbol_table(ast->statement_if.statement_valid);
			break;
			
		case TYPE:
			// Type is terminal but not a var declaration. Do nothing.
			break;
		
		case EXPRESSION_TYPE:
			symbol_table(ast->expression_type.type);
			symbol_table(ast->expression_type.arguments);
			break;
		
		case EXPRESSION_FUNC:
			switch(ast->expression_func.function){
				case 0:
					insert_node("dp3", FUNCTION, ast->expression_func.ln, Scope_num, N, Dummy_count, Test_count[Scope_num]);
				break;
				
				case 1:
					insert_node("lit", FUNCTION, ast->expression_func.ln, Scope_num, N, Dummy_count, Test_count[Scope_num]);
				break;
				
				case 2:
					insert_node("rsq", FUNCTION, ast->expression_func.ln, Scope_num, N, Dummy_count, Test_count[Scope_num]);
				break;
				
				default:
					printf("Invalid EXPRESSION_FUNC type error.\n");
				break;
			}
			symbol_table(ast->expression_func.arguments);
			break;
		
		case EXPRESSION_UNARY:
			// OP is a enum, not a var declaration. Do nothing.
			symbol_table(ast->expression_unary.right);
			break;
			
		case EXPRESSION_BINARY:
			symbol_table(ast->expression_binary.left);
			symbol_table(ast->expression_binary.right);
			break;
		
		case EXPRESSION_BOOL_VALUE:
			// It is a value, not a var declaration. Do nothing.
			break;
		
		case EXPRESSION_INT_VALUE:
			// It is a value, not a var declaration. Do nothing.
			break;
		
		case EXPRESSION_FLOAT_VALUE:
			// It is a value, not a var declaration. Do nothing.
			break;

		case EXPRESSION_BRACKET:
			symbol_table(ast->expression_bracket.expression);
			break;
		
		case ARGUMENTS_MORE_THAN_ONE:
			symbol_table(ast->arguments_more_than_one.arguments_more_than_one);
			symbol_table(ast->arguments_more_than_one.expression);
			break;
		
		case ARGUMENTS_ONLY_ONE:
			symbol_table(ast->arguments_only_one.expression);
			break;
		
		case ARGUMENTS_OPT:
			symbol_table(ast->arguments_opt.arguments);
			break;
		
		case EXPRESSION_VARIABLE:
			symbol_table(ast->expression_variable.variable);
			break;
			
		default:
			break;
	}
}


void freeSymbolTable() {

	L_node *temp = Head;
	while(Head != NULL) {
		temp = Head;
		Head = Head->Next;
		free(temp->Name);
		free(temp);
	}

	return;
}
