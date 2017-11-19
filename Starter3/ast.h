
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

typedef enum {
	PROGRAM = 0,
	SCOPE = 1,
	DECLARATIONS = 2,
	STATEMENTS = 3,
	DECLARATION = 4,
	DECLARATION_ASSIGN = 5,
	DECLARATION_ASSIGN_CONST = 6,
	STATEMENT_ASSIGN = 7,
	STATEMENT_IF_ELSE = 8,
	STATEMENT_IF = 9,
	TYPE = 10,
	EXPRESSION_TYPE = 11,
	EXPRESSION_FUNC = 12,
	EXPRESSION_UNARY = 13,
	EXPRESSION_BINARY = 14,
	EXPRESSION_BOOL_VALUE = 15,
	EXPRESSION_INT_VALUE = 16,
	EXPRESSION_FLOAT_VALUE = 17,
	EXPRESSION_BRACKET = 18,
	VARIABLE = 19,
	ARRAY = 20,
	ARGUMENTS_MORE_THAN_ONE = 21,
	ARGUMENTS_ONLY_ONE = 22,
	ARGUMENTS_OPT = 23,
	EXPRESSION_VARIABLE = 24
} node_kind;

typedef enum {
	INT = 100,
	IVEC2 = 101,
	IVEC3 = 102,
	IVEC4 = 103,
	FLOAT = 104,
	VEC2 = 105,
	VEC3 = 106,
	VEC4 = 107,
	BOOL = 108,
	BVEC2 = 109,
	BVEC3 = 110,
	BVEC4 = 111,
	FUNCTION = 112
} type_kind;

typedef enum {
	UNARY_NEGATIVE = 200,
	UNARY_EXCLAMATION = 201
} unary_op_kind;

typedef enum {
	BINARY_AND = 300,
	BINARY_OR = 301,
	BINARY_EQ = 302,
	BINARY_NEQ = 303,
	BINARY_LT = 304,
	BINARY_LEQ = 305,
	BINARY_GT = 306,
	BINARY_GEQ = 307,
	BINARY_PLUS = 308,
	BINARY_MINUS = 309,
	BINARY_TIMES = 310,
	BINARY_DIVIDE = 311,
	BINARY_XOR = 312
} binary_op_kind;

struct node_ {

	node_kind kind;
	union {
		struct { node *scope; int ln; } program;
		struct { node *declarations; node *statements; int ln; } scope;
		struct { node *declarations; node *declaration; int ln; } declarations;
		struct { node *statements; node *statement; int ln; } statements;
		struct { node *type; char *identifier; int ln; } declaration;
		struct { node *type; char *identifier; node *expression; int ln; } declaration_assign;
		struct { node *type; char *identifier; node *expression; int ln; } declaration_assign_const;
		struct { node *variable; node *expression; int ln; } statement_assign;
		struct { node *expression; node *statement_valid; node *statement_invalid; int ln; } statement_if_else;
		struct { node *expression; node *statement_valid; int ln; } statement_if;
		struct { int type; int ln; } type;
		struct { node *type; node *arguments; int ln; } expression_type;
		struct { int function; node *arguments; int ln; } expression_func;
		struct { int op; node *right; int ln; } expression_unary;
		struct { int op; node *left; node *right; int ln; } expression_binary;
		struct { int bool_value; int ln; } expression_bool_value;
		struct { int int_value; int ln; } expression_int_value;
		struct { double float_value; int ln; } expression_float_value;
		struct { node *expression; int ln; } expression_bracket;
		struct { node *variable; int ln; } expression_variable;
		struct { char *identifier; int ln; } variable;
		struct { char *identifier; int index; int ln; } array;
		struct { node *arguments_more_than_one; node *expression; int ln; } arguments_more_than_one;
		struct { node *expression; int ln; } arguments_only_one;
		struct { node *arguments; int ln; } arguments_opt;
	};
};

node *ast_allocate(node_kind type, ...);
void ast_free(node *ast);
void ast_print(node * ast);

#endif /* AST_H_ */
