#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

#include "ast.h"
#include "common.h"
#include "parser.tab.h"

#define DEBUG_PRINT_TREE 0

node *ast = NULL;

node *ast_allocate(node_kind kind, ...) {
	va_list args;

	// make the node
	node *ast = (node *) malloc(sizeof(node));
	memset(ast, 0, sizeof *ast);
	ast->kind = kind;

  	va_start(args, kind); 

  	switch(kind) {
  
  	case SCOPE:
		ast->scope.declarations = va_arg(args, node*);
		ast->scope.statements = va_arg(args, nodd*);
		break;
	case DECLARATIONS:
		ast->declarations.declarations = va_arg(args, node*);
		ast->declarations.declaration = va_arg(args, node*);
		break;
	case STATEMENTS:
		ast->statements.statements = va_arg(args, node*);
		ast->statements.statement = va_arg(args, node*);
		break;
	case DECLARATION:
		ast->declaration.isConst = va_arg(args, int);
		ast->declaration.type = va_arg(args, node*);
		ast->declaration.ID = va_arg(args, node*);
		ast->declaration.expression = va_arg(args, node*);
		break;
	case STATEMENT_ASSIGN:
		ast->statement_assign.variable = va_arg(args, node*);
		ast->statement_assign.expression = va_arg(args, node*);
		break;
	case STATEMENT_IF:
		ast->statement_if.expression = va_arg(args, node*);
		ast->statement_if.statement_if = va_arg(args, node*);
		ast->statement_if.stattement_else = va_arg(args, node*);
		break;
	case TYPE:
		ast->type.intT = va_arg(args, int);
		ast->type.ivecT = va_arg(args, int);
		ast->type.boolT = va_arg(args, int);
		ast->type.bvecT = va_arg(args, int);
		ast->type.floatT = va_arg(args, int);
		ast->type.vecT = va_arg(args, int);
		break;
	case TYPE_FUNC_EXPR:
		ast->type_func_expr = va_arg(args, node*);
	case BINARY_EXPR:
		ast->binary_expr.op = va_arg(args, int);
		ast->binary_expr.left = va_arg(args, node *);
		ast->binary_expr.right = va_arg(args, node *);
		break;
	case LITERAL_EXPR:
		ast->literal_expr.trueC = va_arg(args, bool);
		ast->literal_expr.falseC = va_arg(args, bool);
		ast->literal_expr.intC = va_arg(args, int);
		ast->literal_expr.floatC = va_arg(args, float);
		break;
	case ARGUMENTS:
		ast->arguments.arguments = va_arg(args, node*);
		ast->arguments.expression = va_arg(args, node*);
		break;
	case ARGUMENTS_OPT:
		ast->arguments_opt.arguments = va_arg(args, node*);
  	default: break;
  	}

  	va_end(args);

  	return ast;
}

void ast_free(node *ast) {

}

void ast_print(node * ast) {

}
