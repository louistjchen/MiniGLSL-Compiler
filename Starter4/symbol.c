/*
 * * Louis Chen - 1000303502
 * Juntu Chen - 1000659799

 *
 *
 * */



#include "symbol.h"

// Global variables
L_node* Head = NULL;
long Scope_num = 0;
long Dummy_count = 0;
long global_dummy_count = 0;

// Linked-list functions
void insert_node(char* Name, int Type, long Line_num, long Scope, attr Attribution, long Count){
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
int is_declared(char* Name, long Scope, long Line_num){
	L_node* Cur;
	Cur = Head;
	while(Cur != NULL){
		if(strcmp(Cur->Name, Name) == 0 && (Cur->Scope <= Scope) && (Cur->Line_num <= Line_num)){
			// At current scope
			if(Cur->Scope == global_dummy_count){
				return Cur->Type;
			}
			else if(Cur->Scope < Scope)
				return Cur->Type;
		}
		Cur = Cur->Next;
	}
	return -1;
}

// This one is for checking declaration for same ID in the same scope. If declaration is not in the same scope, it is valid (shadowing).
int is_existed(char* Name, long Scope, long Line_num){
	L_node* Cur;
	L_node* Temp;
	
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
	printf("You should not get here, you are in trouble!\n");
	return 1;
}

void print_symbol_table(L_node* List_head){
	L_node* Temp;
	Temp = List_head;
	int counter = 0;
	while(Temp != NULL){
		printf("Node %d - Name: %s, Attribution: %d, Value: %d, Type: %d, Ln: %ld, Scope: %ld \n", counter, Temp->Name, Temp->Attribution, Temp->Value, Temp->Type, Temp->Line_num, Temp->Scope);
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
			ast->declaration.ln, Scope_num, N, Dummy_count);
			break;
		
		case DECLARATION_ASSIGN:
			symbol_table(ast->declaration_assign.type);
			
			insert_node(ast->declaration_assign.identifier, ast->declaration_assign.type->type.type,
			ast->declaration_assign.ln, Scope_num, N, Dummy_count);
			
			symbol_table(ast->declaration_assign.expression);
			break;
			
		case DECLARATION_ASSIGN_CONST:
			symbol_table(ast->declaration_assign.type);
			
			insert_node(ast->declaration_assign_const.identifier, 
			ast->declaration_assign_const.type->type.type,
			ast->declaration_assign_const.ln, Scope_num, C, Dummy_count);
			
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
					insert_node("dp3", FUNCTION, ast->expression_func.ln, Scope_num, N, Dummy_count);
				break;
				
				case 1:
					insert_node("lit", FUNCTION, ast->expression_func.ln, Scope_num, N, Dummy_count);
				break;
				
				case 2:
					insert_node("rsq", FUNCTION, ast->expression_func.ln, Scope_num, N, Dummy_count);
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
