/*
 * * Louis Chen - 1000303502
 * Juntu Chen - 1000659799

 *
 *
 * */


#include "semantic.h"

int scopesem = 0;


int semantic_check( node *ast) {

	// base case, return -1
	if(ast == NULL)
		return 0;

	int ret, ret2, numArgs, temp;

  	switch(ast->kind) {

		case PROGRAM:
			scopesem++;
			global_dummy_count++;
			ret = semantic_check(ast->program.scope);
			scopesem--;
			return ret;
			break;
		case SCOPE:
			if(semantic_check(ast->scope.declarations) == -1) { return -1;}
			else if(semantic_check(ast->scope.statements) == -1) { return -1;}
			else return 0;
			break;
		case DECLARATIONS:
			if(semantic_check(ast->declarations.declarations) == -1) { return -1;}
			else if(semantic_check(ast->declarations.declaration) == -1) {return -1;}
			else return 0;
			break;
		case STATEMENTS:
			if(semantic_check(ast->statements.statements) == -1) {return -1;}			else if(semantic_check(ast->statements.statement) == -1) {return -1;}
			else return 0;
			break;
		case DECLARATION:
			temp = is_existed(ast->declaration.identifier, scopesem, ast->declaration.ln);
			// id does not exist in current scope
			if(temp < 0)
				// check its type
				return semantic_check(ast->declaration.type);
			// id already exists in current scope
			else {
				printf("Semantic error (declaration identifier already declared) occurs at line %d\n", ast->declaration.ln);
				return -1;
			}
			break;
		case DECLARATION_ASSIGN:
			ret = semantic_check(ast->declaration_assign.type);
			ret2 = semantic_check(ast->declaration_assign.expression);

			if(ret < 0) return -1;
			else if(ret2 < 0) return -1;

			temp = is_existed(ast->declaration_assign.identifier, scopesem, ast->declaration_assign.ln);
			// id already exists in current scope
			if(temp >= 0) {
				printf("Semantic error (declaration_assign identifier already declared) occurs at line %d\n", ast->declaration_assign.ln);
				return -1;
			}

			// expression assign validity
			if(ast->declaration_assign.expression->kind == VARIABLE) {
				temp = get_attribution(ast->declaration_assign.identifier);
				if(temp == 900) {
					printf("Semantic error (declaration_assign result can't be read) occurs at line %d\n", ast->declaration_assign.ln);
					return -1;
				}
			}

			// assign type validity
			switch(ret2) {
				case INT:
					if(ret == INT || ret == IVEC2 || ret == IVEC3 || ret == IVEC4)
						return ret;
					break;
				case IVEC2:
					if(ret == IVEC2 || ret == IVEC3 || ret == IVEC4)
						return ret;
					break;
				case IVEC3:
					if(ret == IVEC3 || ret == IVEC4)
						return ret;
					break;
				case IVEC4:
					if(ret == IVEC4)
						return ret;
					break;
				case FLOAT:
					if(ret == FLOAT || ret == VEC2 || ret == VEC3 || ret == VEC4)
						return ret;
					break;
				case VEC2:
					if(ret == VEC2 || ret == VEC3 || ret == VEC4)
						return ret;
					break;
				case VEC3:
					if(ret == VEC3 || ret == VEC4)
						return ret;
					break;
				case VEC4:
					if(ret == VEC4)
						return ret;
					break;
				case BOOL:
					if(ret == BOOL || ret == BVEC2 || ret == BVEC3 || ret == BVEC4)
						return ret;
					break;
				case BVEC2:
					if(ret == BVEC2 || ret == BVEC3 || ret == BVEC4)
						return ret;
					break;
				case BVEC3:
					if(ret == BVEC3 || ret == BVEC4)
						return ret;
					break;
				case BVEC4:
					if(ret == BVEC4)
						return ret;
					break;
				default:
					printf("Semantic error (declaration_assign unknown variable type) occurs at line %d\n", ast->declaration_assign.ln);
					return -1;
					break;
			}

			printf("Semantic error (declaration_assign invalid assign combination) occurs at line %d\n", ast->declaration_assign.ln);
			return -1;
			break;
		case DECLARATION_ASSIGN_CONST:

			ret = semantic_check(ast->declaration_assign_const.type);
			ret2 = semantic_check(ast->declaration_assign_const.expression);

			if(ret < 0) return -1;
			else if(ret2 < 0) return -1;

			temp = is_existed(ast->declaration_assign_const.identifier, scopesem, ast->declaration_assign_const.ln);
			// id already exists in current scope
			if(temp >= 0) {
				printf("Semantic error 3(declaration_assign_const identifier already declared) occurs at line %d\n", ast->declaration_assign_const.ln);
				return -1;
			}

			// expression assign validity
			if(ast->declaration_assign_const.expression->kind == VARIABLE) {
				temp = get_attribution(ast->declaration_assign_const.identifier);
				if(temp == 900) {
					printf("Semantic error (declaration_assign_const result can't be read) occurs at line %d\n", ast->declaration_assign_const.ln);
					return -1;
				}
			}

			// assign type validity
			switch(ret2) {
				case INT:
					if(ret == INT || ret == IVEC2 || ret == IVEC3 || ret == IVEC4)
						return ret;
					break;
				case IVEC2:
					if(ret == IVEC2 || ret == IVEC3 || ret == IVEC4)
						return ret;
					break;
				case IVEC3:
					if(ret == IVEC3 || ret == IVEC4)
						return ret;
					break;
				case IVEC4:
					if(ret == IVEC4)
						return ret;
					break;
				case FLOAT:
					if(ret == FLOAT || ret == VEC2 || ret == VEC3 || ret == VEC4)
						return ret;
					break;
				case VEC2:
					if(ret == VEC2 || ret == VEC3 || ret == VEC4)
						return ret;
					break;
				case VEC3:
					if(ret == VEC3 || ret == VEC4)
						return ret;
					break;
				case VEC4:
					if(ret == VEC4)
						return ret;
					break;
				case BOOL:
					if(ret == BOOL || ret == BVEC2 || ret == BVEC3 || ret == BVEC4)
						return ret;
					break;
				case BVEC2:
					if(ret == BVEC2 || ret == BVEC3 || ret == BVEC4)
						return ret;
					break;
				case BVEC3:
					if(ret == BVEC3 || ret == BVEC4)
						return ret;
					break;
				case BVEC4:
					if(ret == BVEC4)
						return ret;
					break;
				default:
					printf("Semantic error (declaration_assign_const unknown variable type) occurs at line %d\n", ast->declaration_assign_const.ln);
					return -1;
					break;
			}

			printf("Semantic error (declaration_assign_const invalid assign combination) occurs at line %d\n", ast->declaration_assign_const.ln);
			return -1;
			break;
		case STATEMENT_ASSIGN:
			ret = semantic_check(ast->statement_assign.variable);
			ret2 = semantic_check(ast->statement_assign.expression);

			if(ret < 0) return -1;
			else if(ret2 < 0) return -1;

			temp = is_declared( ast->statement_assign.variable->variable.identifier , scopesem, ast->statement_assign.ln);
			if(temp < 0) {
				printf("Semantic error (statement_assign variable name not declared) occurs at line %d\n", ast->statement_assign.ln);
				return -1;
			}

			// right variable assign validity
			if(ast->statement_assign.expression->kind == VARIABLE) {
				temp = get_attribution(ast->statement_assign.expression->variable.identifier);
				if(temp == 900) {
					printf("Semantic error (statement_assign result can't be read) occurs at line %d\n", ast->statement_assign.ln);
					return -1;
				}
			}
			// left variable assign validity
			if(ast->statement_assign.variable->kind == VARIABLE) {
				temp = get_attribution(ast->statement_assign.variable->variable.identifier);
				if(temp == 901 || temp == 902 || temp == 904) {
					printf("Semantic error (statement_assign write to const) occurs at line %d\n", ast->statement_assign.ln);
					return -1;
				}
			}

			// assign type validity
			switch(ret2) {
				case INT:
					if(ret == INT || ret == IVEC2 || ret == IVEC3 || ret == IVEC4)
						return ret;
					break;
				case IVEC2:
					if(ret == IVEC2 || ret == IVEC3 || ret == IVEC4)
						return ret;
					break;
				case IVEC3:
					if(ret == IVEC3 || ret == IVEC4)
						return ret;
					break;
				case IVEC4:
					if(ret == IVEC4)
						return ret;
					break;
				case FLOAT:
					if(ret == FLOAT || ret == VEC2 || ret == VEC3 || ret == VEC4)
						return ret;
					break;
				case VEC2:
					if(ret == VEC2 || ret == VEC3 || ret == VEC4)
						return ret;
					break;
				case VEC3:
					if(ret == VEC3 || ret == VEC4)
						return ret;
					break;
				case VEC4:
					if(ret == VEC4)
						return ret;
					break;
				case BOOL:
					if(ret == BOOL || ret == BVEC2 || ret == BVEC3 || ret == BVEC4)
						return ret;
					break;
				case BVEC2:
					if(ret == BVEC2 || ret == BVEC3 || ret == BVEC4)
						return ret;
					break;
				case BVEC3:
					if(ret == BVEC3 || ret == BVEC4)
						return ret;
					break;
				case BVEC4:
					if(ret == BVEC4)
						return ret;
					break;
				default:
					printf("Semantic error (statement_assign unknown variable type) occurs at line %d\n", ast->statement_assign.ln);
					return -1;
					break;
			}

			printf("Semantic error (statement_assign invalid assign combination) occurs at line %d\n", ast->statement_assign.ln);
			return -1;
			break;
		case STATEMENT_IF_ELSE:
			ret = semantic_check(ast->statement_if_else.expression);
			if(ret < 0) return -1;

			if(ret != BOOL) {
				printf("Semantic error (statement_if_else condition not boolean type) occurs at line %d\n", ast->statement_if_else.ln);
				return -1;
			}
			else if(semantic_check(ast->statement_if_else.statement_valid) < 0)
				return -1;
			else if(semantic_check(ast->statement_if_else.statement_invalid) < 0)
				return -1;
			else
				return ret;
			break;
		case STATEMENT_IF:
			ret = semantic_check(ast->statement_if.expression);
			if(ret < 0) return -1;

			if(ret != BOOL) {
				printf("Semantic error (statement_if condition not boolean type) occurs at line %d\n", ast->statement_if.ln);
				return -1;
			}
			else if(semantic_check(ast->statement_if.statement_valid) < 0)
				return -1;
			else
				return ret;
			break;
		case TYPE:
			return ast->type.type;
			break;
		case EXPRESSION_TYPE:
			ret =  semantic_check(ast->expression_type.type);
			ret2 = semantic_check(ast->expression_type.arguments);
			if(ret < 0) return ret;
			else if(ret2 < 0) return ret2;

			numArgs = numArguments(ast->expression_type.arguments);
			if(ret == INT || ret == FLOAT || ret == BOOL) {
				if(numArgs == 1 && ret == ret2)
					return ret;
				else if(ret != ret2){
					printf("Semantic error (expression_type assign wrong scalar type to a scalar) occurs at line %d\n", ast->expression_type.ln);
					return -1;
				}
				else{
					printf("Semantic error (expression_type assign a vector to a scalar type) occurs at line %d\n", ast->expression_type.ln);
					return -1;
				}
			}
			else if(ret == IVEC2 || ret == VEC2 || ret == BVEC2) {
				if(numArgs == 2)
					if(ret == IVEC2 && ret2 == INT)
						return ret;
					else if(ret == VEC2 && ret2 == FLOAT)
						return ret;
					else if(ret == BVEC2 && ret2 == BOOL)
						return ret;
					else {	
						printf("Semantic error (expression_type assign wrong i/b/vec2 type to a i/b/vec2) occurs at line %d\n", ast->expression_type.ln);
						return -1;
					}
				else {
					printf("Semantic error (expression_type assign not 2 args for IVEC2/BVEC2/VEC2) occurs at line %d\n", ast->expression_type.ln);
					return -1;
				}
			}
			else if(ret == IVEC3 || ret == VEC3 || ret == BVEC3) {
				if(numArgs == 3)
					if(ret == IVEC3 && ret2 == INT)
						return ret;
					else if(ret == VEC3 && ret2 == FLOAT)
						return ret;
					else if(ret == BVEC3 && ret2 == BOOL)
						return ret;
					else {	
						printf("Semantic error (expression_type assign wrong i/b/vec3 type to a i/b/vec3) occurs at line %d\n", ast->expression_type.ln);
						return -1;
					}
				else {
					printf("Semantic error (expression_type assign not 3 args for IVEC3/BVEC3/VEC3) occurs at line %d\n", ast->expression_type.ln);
					return -1;
				}
			}
			else if(ret == IVEC4 || ret == VEC4 || ret == BVEC4) {
				if(numArgs == 4)
					if(ret == IVEC4 && ret2 == INT)
						return ret;
					else if(ret == VEC4 && ret2 == FLOAT)
						return ret;
					else if(ret == BVEC4 && ret2 == BOOL)
						return ret;
					else {	
						printf("Semantic error (expression_type assign wrong i/b/vec4 type to a i/b/vec4) occurs at line %d\n", ast->expression_type.ln);
						return -1;
					}
				else {
					printf("Semantic error (expression_type assign not 4 args for IVEC4/BVEC4/VEC4) occurs at line %d\n", ast->expression_type.ln);
					return -1;
				}
			}
			else {
				printf("Semantic error (expression_type unknown type) occurs at line %d. If you see this, then you're in trouble\n", ast->expression_type.ln);
				return -1;
			}

			break;
		case EXPRESSION_FUNC:
			ret = semantic_check(ast->expression_func.arguments);
			if(ret < 0) return -1;

			switch(ast->expression_func.function){
				// rsq
				case 2:
					if(ret == FLOAT || ret == INT)
						return FLOAT;
					else {
						printf("Semantic error (expression_func rsq argument not FLOAT or INT) occurs at line %d\n", ast->expression_func.ln);
						return -1;
					}
					break;
				// dp3
				case 0:
					if(ret == VEC3 || ret == VEC4)
						return FLOAT;
					else if(ret == IVEC3 || ret == IVEC4)
						return INT;
					else {
						printf("Semantic error (expression_func dp3 argument not IVEC34 or VEC34) occurs at line %d\n", ast->expression_func.ln);
						return -1;
					}
					break;
				// lit
				case 1:
					if(ret == VEC4)
						return VEC4;
					else {
						printf("Semantic error (expression_func lit argument not VEC4) occurs at line %d\n", ast->expression_func.ln);
						return -1;
					}
					break;
				default:
					printf("Semantic error (expression_func unknown function) occurs at line %d\n", ast->expression_func.ln);
					return -1;
					break;
			}
			break;
		case EXPRESSION_UNARY:
			ret = semantic_check(ast->expression_unary.right);
			if(ret < 0) return -1;

			if(ast->expression_unary.op == UNARY_NEGATIVE) {
				if(ret == BOOL || ret == BVEC2 || ret == BVEC3 || ret == BVEC4) {
					printf("Semantic error (expression_unary -) occurs at line %d\n", ast->expression_unary.ln);
					return -1;
				}
			}
			else if(ast->expression_unary.op == UNARY_EXCLAMATION) {
				if(ret != BOOL && ret != BVEC2 && ret != BVEC3 && ret != BVEC4) {
					printf("Semantic error (expression_unary !) occurs at line %d\n", ast->expression_unary.ln);
					return -1;
				}
			}
			else {
				printf("Semantic error (expression_unary \"unknown op\") occurs at line %d\n", ast->expression_unary.ln);
				return -1;
			}

			return ret;
			break;
		case EXPRESSION_BINARY:
			ret = semantic_check(ast->expression_binary.left);
			ret2 = semantic_check(ast->expression_binary.right);
			if(ret < 0) return ret;
			else if(ret2 < 0) return ret2;

			if(ast->expression_binary.op == BINARY_AND || ast->expression_binary.op == BINARY_OR) {
				if(ret == ret2) {
					if(ret == BOOL || ret == BVEC2 || ret == BVEC3 || ret == BVEC4)
						return BOOL;
					else {
						printf("Semantic error (expression_binary && || invalid type) occurs at line %d\n", ast->expression_binary.ln);
						return -1;
					}
				}
				else {
					printf("Semantic error (expression_binary && || left-right don't match) occurs at line %d\n", ast->expression_binary.ln);
					return -1;
				}
			}
			else if(ast->expression_binary.op == BINARY_LT || ast->expression_binary.op == BINARY_LEQ || ast->expression_binary.op == BINARY_GT || ast->expression_binary.op == BINARY_GEQ) {
				// left right same type
				if(ret == ret2) {
					// left right both int or float, scalar
					if(ret == INT || ret == FLOAT)
						return BOOL;
					else {
						printf("Semantic error (expression_binary < <= > >= invalid type \"not INT or FLOAT\") occurs at line %d\n", ast->expression_binary.ln);
						return -1;
					}
				}
				else {
					printf("Semantic error (expression_binary < <= > >= left-right don't match) occurs at line %d\n", ast->expression_binary.ln);
					return -1;
				}
			}
			else if(ast->expression_binary.op == BINARY_EQ || ast->expression_binary.op == BINARY_NEQ) {
				// left right same type
				if(ret == ret2) {
					// left right both int or float, both scalar or vector
					if(ret == INT || ret == IVEC2 || ret == IVEC3 || ret == IVEC4 || ret == FLOAT || ret == VEC2 || ret == VEC3 || ret == VEC4)
						return BOOL;
					else {
						printf("Semantic error (expression_binary == != invalid type \"not INT or FLOAT or their vector types\" occurs at line %d\n", ast->expression_binary.ln);
						return -1;
					}
				}
				else {
					printf("Semantic error (expression_binary == != left-right don't match) occurs at line %d\n", ast->expression_binary.ln);
					return -1;
				}
			}
			else if(ast->expression_binary.op == BINARY_PLUS || ast->expression_binary.op == BINARY_MINUS) {
				// left right same type
				if(ret == ret2) {
					// left right both int or float, both scalar or vector
					if(ret == INT || ret == IVEC2 || ret == IVEC3 || ret == IVEC4 || ret == FLOAT || ret == VEC2 || ret == VEC3 || ret == VEC4)
						return ret;
					else {
						printf("Semantic error (expression_binary + - invalid type \"not INT or FLOAT or their vector types\" occurs at line %d\n", ast->expression_binary.ln);
						return -1;
					}
				}
				else {
					printf("Semantic error (expression_binary + - left-right don't match) occurs at line %d\n", ast->expression_binary.ln);
					return -1;
				}
			}
			else if(ast->expression_binary.op == BINARY_TIMES) {
				if(ret == ret2) {
					if(ret == BOOL || ret == BVEC2 || ret == BVEC3 || ret == BVEC4) {
						printf("Semantic error (expression_binary * left right same but bool234) occurs at line %d\n", ast->expression_binary.ln);
						return -1;
					}
					else
						return ret;
				}
				else {
					if(ret == INT && (ret2 == IVEC2 || ret2 == IVEC3 || ret2 == IVEC4))
						return ret2;
					else if(ret2 == INT && (ret == IVEC2 || ret == IVEC3 || ret == IVEC4))
						return ret;
					else if(ret == FLOAT && (ret2 == VEC2 || ret2 == VEC3 || ret2 == VEC4))
						return ret2;
					else if(ret2 == FLOAT && (ret == VEC2 || ret == VEC3 || ret == VEC4))
						return ret;
					else {
						printf("Semantic error (expression_binary * left right invalid combination) occurs at line %d\n", ast->expression_binary.ln);
						return -1;
					}
				}
			}
			else if(ast->expression_binary.op == BINARY_DIVIDE || ast->expression_binary.op == BINARY_XOR) {
				// left and right same
				if(ret == ret2 && (ret == INT || ret == FLOAT))
					return ret;
				else {
					printf("Semantic error (expression_binary / ^ left right not both int or both float) occurs at line %d\n", ast->expression_binary.ln);
					return -1;
				}
			}
			else {
				printf("Semantic error (expression_binary unknown) occurs at line %d\n", ast->expression_binary.ln);
				return -1;
			}
			break;
		case EXPRESSION_BOOL_VALUE:
			return BOOL;
			break;
		case EXPRESSION_INT_VALUE:
			return INT;
			break;
		case EXPRESSION_FLOAT_VALUE:
			return FLOAT;
			break;
		case EXPRESSION_BRACKET:
			return semantic_check(ast->expression_bracket.expression);
			break;
		case EXPRESSION_VARIABLE:
			return semantic_check(ast->expression_variable.variable);			
			break;
		case VARIABLE:
			ret = is_declared(ast->variable.identifier, scopesem, ast->variable.ln);
			if(ret < 0) {
				printf("Semantic error (variable not declared) occurs at line %d\n", ast->variable.ln);
				return -1;
			}
			else
				return ret;
			break;
		case ARRAY:
			ret = is_declared(ast->array.identifier, scopesem, ast->array.ln);
			if(ret < 0) {
				printf("Semantic error (vector not declared) occurs at line %d\n", ast->array.ln);
				return -1;
			}
			else {
				// ret2 stores index of array
				ret2 = ast->array.index;

				// IVEC2 BVEC2 VEC2
				if(ret == IVEC2 || ret == BVEC2 || ret == VEC2) {
					// index is within 0 to 1
					if(ret2 < 2)
						if(ret == IVEC2)
							return INT;
						else if(ret == BVEC2)
							return BOOL;
						else if(ret == VEC2)
							return FLOAT;
						else {
							printf("Semantic error (vector index unknown vector type) occurs at line %d. Should never get here!\n", ast->array.ln);
						}
					// index exceeds 1
					else {
						printf("Semantic error (vector index out of range) occurs at line %d\n", ast->array.ln);
						return -1;
					}
				}
				// IVEC3 BVEC3 VEC3
				else if(ret == IVEC3 || ret == BVEC3 || ret == VEC3) {
					// index is within 0 to 2
					if(ret2 < 3)
						if(ret == IVEC3)
							return INT;
						else if(ret == BVEC3)
							return BOOL;
						else if(ret == VEC3)
							return FLOAT;
						else {
							printf("Semantic error (vector index unknown vector type) occurs at line %d. Should never get here!\n", ast->array.ln);
						}
					// index exceeds 2
					else {
						printf("Semantic error (vector index out of range) occurs at line %d\n", ast->array.ln);
						return -1;
					}
				}
				// IVEC4 BVEC4 VEC4
				else if(ret == IVEC4 || ret == BVEC4 || ret == VEC4) {
					// index is within 0 to 3
					if(ret2 < 4)
						if(ret == IVEC4)
							return INT;
						else if(ret == BVEC4)
							return BOOL;
						else if(ret == VEC4)
							return FLOAT;
						else {
							printf("Semantic error (vector index unknown vector type) occurs at line %d. Should never get here!\n", ast->array.ln);
						}
					// index exceeds 3
					else {
						printf("Semantic error (vector index out of range) occurs at line %d\n", ast->array.ln);
						return -1;
					}
				}
				// not vector
				else {
					printf("Semantic error (indexing a non-vector type) occurs at line %d\n", ast->array.ln);
					return -1;
				}
			}
			break;
		case ARGUMENTS_MORE_THAN_ONE:
			ret = semantic_check(ast->arguments_more_than_one.arguments_more_than_one);
			ret2 = semantic_check(ast->arguments_more_than_one.expression);
			if(ret < 0) return -1;
			else if(ret2 < 0) return -1;

			// if both arguments same type, then return their types
			if(ret == ret2)
				return ret;
			// else report error because types in arguments must be the same
			else {
				printf("Semantic error (arguments_more_than_one arguments' types don't match) occurs at line %d\n", ast->arguments_more_than_one.ln);
				return -1;
			}
			break;
		case ARGUMENTS_ONLY_ONE:
			return semantic_check(ast->arguments_only_one.expression);
			break;
		case ARGUMENTS_OPT:
			return semantic_check(ast->arguments_opt.arguments);
			break;
		default:
			printf("Semantic error (default case should not enter here) occurs");
			return -1;
			break;
  	}      


	return 0; // failed checks
}



int numArguments(node *ast) {

	// base case
	if (ast == NULL)
		return -1;

	if(ast->kind == ARGUMENTS_MORE_THAN_ONE)
		return numArguments(ast->arguments_more_than_one.arguments_more_than_one) + 1;
	else if(ast->kind == ARGUMENTS_OPT)
		return numArguments(ast->arguments_opt.arguments);
	else if(ast->kind >= PROGRAM && ast->kind <= ARGUMENTS_ONLY_ONE)
		return 1;
	else {
		printf("Semantic error (undefined node type)\n");
		return -1;
	}
}
