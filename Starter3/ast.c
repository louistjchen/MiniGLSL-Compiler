/*
 * * Louis Chen - 1000303502
 * Juntu Chen - 1000659799

 *
 * */




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

#include "ast.h"
#include "common.h"
#include "parser.tab.h"

#define DEBUG_PRINT_TREE 0

node *ast = NULL;
int sscope = 0;

node *ast_allocate(node_kind kind, ...) {
	va_list args;

	// make the node
	node *ast = (node *) malloc(sizeof(node));
	memset(ast, 0, sizeof *ast);
	ast->kind = kind;

  	va_start(args, kind); 

  	switch(kind) {

		case PROGRAM:
			ast->program.scope = va_arg(args, node*);
			ast->program.ln = va_arg(args, int);
			break;
		case SCOPE:
			ast->scope.declarations = va_arg(args, node*);
			ast->scope.statements = va_arg(args, node*);
			ast->scope.ln = va_arg(args, int);
			break;
		case DECLARATIONS:
			ast->declarations.declarations = va_arg(args, node*);
			ast->declarations.declaration = va_arg(args, node*);
			ast->declarations.ln = va_arg(args, int);
			break;
		case STATEMENTS:
			ast->statements.statements = va_arg(args, node*);
			ast->statements.statement = va_arg(args, node*);
			ast->statements.ln = va_arg(args, int);
			break;
		case DECLARATION:
			ast->declaration.type = va_arg(args, node*);
			ast->declaration.identifier = va_arg(args, char*);
			ast->declaration.ln = va_arg(args, int);
			break;
		case DECLARATION_ASSIGN:
			ast->declaration_assign.type = va_arg(args, node*);
			ast->declaration_assign.identifier = va_arg(args, char*);
			ast->declaration_assign.expression = va_arg(args, node*);
			ast->declaration_assign.ln = va_arg(args, int);
			break;
		case DECLARATION_ASSIGN_CONST:
			ast->declaration_assign_const.type = va_arg(args, node*);
			ast->declaration_assign_const.identifier = va_arg(args, char*);
			ast->declaration_assign_const.expression = va_arg(args, node*);
			ast->declaration_assign_const.ln = va_arg(args, int);
			break;
		case STATEMENT_ASSIGN:
			ast->statement_assign.variable = va_arg(args, node*);
			ast->statement_assign.expression = va_arg(args, node*);
			ast->statement_assign.ln = va_arg(args, int);
			break;
		case STATEMENT_IF_ELSE:
			ast->statement_if_else.expression = va_arg(args, node*);
			ast->statement_if_else.statement_valid = va_arg(args, node*);
			ast->statement_if_else.statement_invalid = va_arg(args, node*);
			ast->statement_if_else.ln = va_arg(args, int);
			break;
		case STATEMENT_IF:
			ast->statement_if.expression = va_arg(args, node*);
			ast->statement_if.statement_valid = va_arg(args, node*);
			ast->statement_if.ln = va_arg(args, int);
			break;
		case TYPE:
			ast->type.type = va_arg(args, int);
			ast->type.ln = va_arg(args, int);
			break;
		case EXPRESSION_TYPE:
			ast->expression_type.type = va_arg(args, node*);
			ast->expression_type.arguments = va_arg(args, node*);
			ast->expression_type.ln = va_arg(args, int);
			break;
		case EXPRESSION_FUNC:
			ast->expression_func.function = va_arg(args, int);
			ast->expression_func.arguments = va_arg(args, node*);
			ast->expression_func.ln = va_arg(args, int);
			break;
		case EXPRESSION_UNARY:
			ast->expression_unary.op = va_arg(args, int);
			ast->expression_unary.right = va_arg(args, node*);
			ast->expression_unary.ln = va_arg(args, int);
			break;
		case EXPRESSION_BINARY:
			ast->expression_binary.op = va_arg(args, int);
			ast->expression_binary.left = va_arg(args, node*);
			ast->expression_binary.right = va_arg(args, node*);
			ast->expression_binary.ln = va_arg(args, int);
			break;
		case EXPRESSION_BOOL_VALUE:
			ast->expression_bool_value.bool_value = va_arg(args, int);
			ast->expression_bool_value.ln = va_arg(args, int);
			break;
		case EXPRESSION_INT_VALUE:
			ast->expression_int_value.int_value = va_arg(args, int);
			ast->expression_int_value.ln = va_arg(args, int);
			break;
		case EXPRESSION_FLOAT_VALUE:
			ast->expression_float_value.float_value = va_arg(args, double);
			ast->expression_float_value.ln = va_arg(args, int);
			break;
		case EXPRESSION_BRACKET:
			ast->expression_bracket.expression = va_arg(args, node*);
			ast->expression_bracket.ln = va_arg(args, int);
			break;
		case EXPRESSION_VARIABLE:
			ast->expression_variable.variable = va_arg(args, node*);
			ast->expression_variable.ln = va_arg(args, int);
			break;
		case VARIABLE:
			ast->variable.identifier = va_arg(args, char*);
			ast->variable.ln = va_arg(args, int);
			break;
		case ARRAY:
			ast->array.identifier = va_arg(args, char*);
			ast->array.index = va_arg(args, int);
			ast->array.ln = va_arg(args, int);
			break;
		case ARGUMENTS_MORE_THAN_ONE:
			ast->arguments_more_than_one.arguments_more_than_one = va_arg(args, node*);
			ast->arguments_more_than_one.expression = va_arg(args, node*);
			ast->arguments_more_than_one.ln = va_arg(args, int);
			break;
		case ARGUMENTS_ONLY_ONE:
			ast->arguments_only_one.expression = va_arg(args, node*);
			ast->arguments_only_one.ln = va_arg(args, int);
			break;
		case ARGUMENTS_OPT:
			ast->arguments_opt.arguments = va_arg(args, node*);
			break;
		default:
			break;
  	}

  	va_end(args);

  	return ast;
}

void ast_free(node *ast) {

	// base case
	if(ast == NULL)
		return;

  	switch(ast->kind) {

		case PROGRAM:
			ast_free(ast->program.scope);
			break;
		case SCOPE:
			ast_free(ast->scope.declarations);
			ast_free(ast->scope.statements);
			break;
		case DECLARATIONS:
			ast_free(ast->declarations.declarations);
			ast_free(ast->declarations.declaration);
			break;
		case STATEMENTS:
			ast_free(ast->statements.statements);
			ast_free(ast->statements.statement);
			break;
		case DECLARATION:
			ast_free(ast->declaration.type);
			break;
		case DECLARATION_ASSIGN:
			ast_free(ast->declaration_assign.type);
			ast_free(ast->declaration_assign.expression);
			break;
		case DECLARATION_ASSIGN_CONST:
			ast_free(ast->declaration_assign_const.type);
			ast_free(ast->declaration_assign_const.expression);
			break;
		case STATEMENT_ASSIGN:
			ast_free(ast->statement_assign.variable);
			ast_free(ast->statement_assign.expression);
			break;
		case STATEMENT_IF_ELSE:
			ast_free(ast->statement_if_else.expression);
			ast_free(ast->statement_if_else.statement_valid);
			ast_free(ast->statement_if_else.statement_invalid);
			break;
		case STATEMENT_IF:
			ast_free(ast->statement_if.expression);
			ast_free(ast->statement_if.statement_valid);
			break;
		case TYPE:
			break;
		case EXPRESSION_TYPE:
			ast_free(ast->expression_type.arguments);
			break;
		case EXPRESSION_FUNC:
			ast_free(ast->expression_func.arguments);
			break;
		case EXPRESSION_UNARY:
			ast_free(ast->expression_unary.right);
			break;
		case EXPRESSION_BINARY:
			ast_free(ast->expression_binary.left);
			ast_free(ast->expression_binary.right);
			break;
		case EXPRESSION_BOOL_VALUE:
			break;
		case EXPRESSION_INT_VALUE:
			break;
		case EXPRESSION_FLOAT_VALUE:
			break;
		case EXPRESSION_BRACKET:
			ast_free(ast->expression_bracket.expression);
			break;
		case EXPRESSION_VARIABLE:
			ast_free(ast->expression_variable.variable);
			break;
		case VARIABLE:
			break;
		case ARRAY:
			break;
		case ARGUMENTS_MORE_THAN_ONE:
			ast_free(ast->arguments_more_than_one.arguments_more_than_one);
			ast_free(ast->arguments_more_than_one.expression);
			break;
		case ARGUMENTS_ONLY_ONE:
			ast_free(ast->arguments_only_one.expression);
			break;
		case ARGUMENTS_OPT:
			ast_free(ast->arguments_opt.arguments);
		default:
			break;
  	}

	if(ast != NULL)
		free(ast);
	ast = NULL;
}

void ast_print(node * ast) {

	// base case
	if(ast == NULL)
		return;

  	switch(ast->kind) {

		case PROGRAM:
			printf("\n ( SCOPE ");
			ast_print(ast->program.scope);
			printf(" ) \n");
			break;
		case SCOPE:
			ast_print(ast->scope.declarations);
			ast_print(ast->scope.statements);
			break;
		case DECLARATIONS:
			printf("\n ( DECLARATIONS ");
			ast_print(ast->declarations.declarations);
			ast_print(ast->declarations.declaration);
			printf(" ) \n");
			break;
		case STATEMENTS:
			printf("\n ( STATEMENTS ");
			ast_print(ast->statements.statements);
			ast_print(ast->statements.statement);
			printf(" ) \n");
			break;
		case DECLARATION:
			printf("\n ( DECLARATION %s ", ast->declaration.identifier);
			ast_print(ast->declaration.type);
			printf(" ) \n");
			break;
		case DECLARATION_ASSIGN:
			printf("\n ( DECLARATION %s ", ast->declaration.identifier);
			ast_print(ast->declaration_assign.type);
			ast_print(ast->declaration_assign.expression);
			printf(" ) \n");
			break;
		case DECLARATION_ASSIGN_CONST:
			printf("\n ( DECLARATION %s ", ast->declaration.identifier);
			ast_print(ast->declaration_assign_const.type);
			ast_print(ast->declaration_assign_const.expression);
			printf(" ) \n");
			break;
		case STATEMENT_ASSIGN:
			printf("\n ( ASSIGN ");
			ast_print(ast->statement_assign.variable);
			ast_print(ast->statement_assign.expression);
			printf(" ) \n");
			break;
		case STATEMENT_IF_ELSE:
			printf("\n ( IF ");
			ast_print(ast->statement_if_else.expression);
			ast_print(ast->statement_if_else.statement_valid);
			ast_print(ast->statement_if_else.statement_invalid);
			printf(" ) \n");
			break;
		case STATEMENT_IF:
			printf("\n ( IF ");
			ast_print(ast->statement_if.expression);
			ast_print(ast->statement_if.statement_valid);
			printf(" ) \n");
			break;
		case TYPE:
			switch(ast->type.type) {
				case INT: printf(" INT "); break;
				case IVEC2: printf(" IVEC2 "); break;
				case IVEC3: printf(" IVEC3 "); break;
				case IVEC4: printf(" IVEC4 "); break;
				case BOOL: printf(" BOOL "); break;
				case BVEC2: printf(" BVEC2 "); break;
				case BVEC3: printf(" BVEC3 "); break;
				case BVEC4: printf(" BVEC4 "); break;
				case FLOAT: printf(" FLOAT "); break;
				case VEC2: printf(" VEC2 "); break;
				case VEC3: printf(" VEC3 "); break;
				case VEC4: printf(" VEC4 "); break;
				case FUNCTION: printf(" FUNC "); break;
				default: printf(" DEFAULT-TYPE%d ", ast->type.type); break;
			}
			break;
		case EXPRESSION_TYPE:
			printf("\n (CALL ");
			ast_print(ast->expression_type.type);
			ast_print(ast->expression_type.arguments);
			printf(" ) \n");
			break;
		case EXPRESSION_FUNC:
			printf("\n (CALL ");
			switch(ast->expression_func.function) {
				case 0: printf(" DP3 "); break;
				case 1: printf(" LIT "); break;
				case 2: printf(" RSQ "); break;
				default: printf(" DEFAULT-FUNC "); break;
			}
			ast_print(ast->expression_func.arguments);
			printf(" ) \n");
			break;
		case EXPRESSION_UNARY:
			printf("\n (UNARY ");
			switch(ast->expression_unary.op) {
				case UNARY_NEGATIVE: printf("- "); break;
				case UNARY_EXCLAMATION: printf("! "); break;
				default: printf("DEFAULT-UNARY"); break;
			}
			ast_print(ast->expression_unary.right);
			printf(" ) \n");
			break;
		case EXPRESSION_BINARY:
			printf("\n (BINARY ");
			switch(ast->expression_binary.op) {
				case BINARY_AND: printf("&& "); break;
				case BINARY_OR: printf("|| "); break;
				case BINARY_EQ: printf("== "); break;
				case BINARY_NEQ: printf("!= "); break;
				case BINARY_LT: printf("< "); break;
				case BINARY_LEQ: printf("<= "); break;
				case BINARY_GT: printf("> "); break;
				case BINARY_GEQ: printf(">= "); break;
				case BINARY_PLUS: printf("+ "); break;
				case BINARY_MINUS: printf("- "); break;
				case BINARY_TIMES: printf("* "); break;
				case BINARY_DIVIDE: printf("/ "); break;
				case BINARY_XOR: printf("^ "); break;
				default: printf("DEFAULT-BINARY "); break;
			}
			ast_print(ast->expression_binary.left);
			ast_print(ast->expression_binary.right);
			printf(" ) \n ");
			break;
		case EXPRESSION_BOOL_VALUE:
			if(ast->expression_bool_value.bool_value == 0)
				printf(" false ");
			else
				printf(" true ");
			break;
		case EXPRESSION_INT_VALUE:
			printf(" %d ", ast->expression_int_value.int_value);
			break;
		case EXPRESSION_FLOAT_VALUE:
			printf(" %f ", ast->expression_float_value.float_value);
			break;
		case EXPRESSION_BRACKET:
			ast_print(ast->expression_bracket.expression);
			break;
		case EXPRESSION_VARIABLE:
			ast_print(ast->expression_variable.variable);
			break;
		case VARIABLE:
			printf(" %s ", ast->variable.identifier);
			break;
		case ARRAY:
			printf("\n (INDEX %s %d ) \n", ast->array.identifier, ast->array.index);
			break;
		case ARGUMENTS_MORE_THAN_ONE:
			ast_print(ast->arguments_more_than_one.arguments_more_than_one);
			ast_print(ast->arguments_more_than_one.expression);
			break;
		case ARGUMENTS_ONLY_ONE:
			ast_print(ast->arguments_only_one.expression);
			break;
		case ARGUMENTS_OPT:
			ast_print(ast->arguments_opt.arguments);
		default:
			break;
  	}

}
