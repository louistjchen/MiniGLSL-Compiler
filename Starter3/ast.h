
#ifndef AST_H_
#define AST_H_ 1

#include <stdarg.h>

// Dummy node just so everything compiles, create your own node/nodes
//
// The code provided below is an example ONLY. You can use/modify it,
// but do not assume that it is correct or complete.
//
// There are many ways of making AST nodes. The approach below is an example
// of a descriminated union. If you choose to use C++, then I suggest looking
// into inheritance.

// forward declare
struct node_;
typedef struct node_ node;
extern node *ast;
/*
typedef enum {
  UNKNOWN               = 0,

  SCOPE_NODE            = (1 << 0),
  
  EXPRESSION_NODE       = (1 << 2),
  UNARY_EXPRESION_NODE  = (1 << 2) | (1 << 3),
  BINARY_EXPRESSION_NODE= (1 << 2) | (1 << 4),
  INT_NODE              = (1 << 2) | (1 << 5), 
  FLOAT_NODE            = (1 << 2) | (1 << 6),
  IDENT_NODE            = (1 << 2) | (1 << 7),
  VAR_NODE              = (1 << 2) | (1 << 8),
  FUNCTION_NODE         = (1 << 2) | (1 << 9),
  CONSTRUCTOR_NODE      = (1 << 2) | (1 << 10),

  STATEMENT_NODE        = (1 << 1),
  IF_STATEMENT_NODE     = (1 << 1) | (1 << 11),
  WHILE_STATEMENT_NODE  = (1 << 1) | (1 << 12),
  ASSIGNMENT_NODE       = (1 << 1) | (1 << 13),
  NESTED_SCOPE_NODE     = (1 << 1) | (1 << 14),

  DECLARATION_NODE      = (1 << 15),
  DECLARATIONS_NODE	= (1 << 16)
} node_kind;
*/

typedef enum {
	SCOPE = 0,
	DECLARATIONS = 1,
	STATEMENTS = 2,
	DECLARATION = 3,
	STATEMENT_ASSIGN = 4,
	STATEMENT_IF = 5,
	TYPE = 6,
	TYPE_FUNC_EXPR = 7,
	UNARY_EXPR = 8,
	BINARY_EXPR = 9,
	LITERAL_EXPR = 10,
	ARGUMENTS = 11,
	ARGUMENTS_OPT = 12
} node_kind;

struct node_ {

	node_kind kind;

	union {
		struct { node *declarations; node *statements; } scope;
		struct { node *declarations; node *declaration; } declarations;
		struct { node *statements; node *statement; } statements;
		struct { int isConst; node *type; node *ID; node *expression; } declaration;
		struct { node *variable; node *expression; } statement_assign;
		struct { node *expression; node *statement_if; node *statement_else; } statement_if;
		struct { int intT; int ivecT; int boolT; int bvecT; int floatT; int vecT; } type;
		struct { node *type_func; node *arguments_opt; } type_func_expr;
		struct { int op; node *right; } unary_expr;
		struct { int op; node *left; node *right; } binary_expr;
		struct { bool trueC; bool falseC; int intC; float floatC; } literal_expr;
		struct { node *arguments; node *expression; } arguments;
		struct { node *arguments; } arguments_opt;
	};
};

node *ast_allocate(node_kind type, ...);
void ast_free(node *ast);
void ast_print(node * ast);

#endif /* AST_H_ */
