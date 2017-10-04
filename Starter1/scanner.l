%{
/**********************************************************************
 *
 * **YOUR GROUP INFO SHOULD GO HERE**
 *
 *  Interface to the scanner module for CSC467 course project
 **********************************************************************/


#include "common.h"
#include "parser.tab.h"
#include "string.h"
#include <limits.h>
#include <float.h>


#define YY_USER_INIT { yyin = inputFile; }
#define	yyinput      input
#define yTRACE(x)    { if (traceScanner) fprintf(traceFile, "TOKEN %3d : %s\n", x, yytext); }
#define yERROR(x)    { fprintf(errorFile, "\nLEXICAL ERROR, LINE %d: %s\n", yyline, x); errorOccurred = TRUE; yyterminate(); }

int CheckInt(void);
int yyline = 1;
int processStarComment();
int processSlashComment();
int processID(char *yytext, int tokenKind);
int processLit(char *yytext, int tokenKind);


%}
%option noyywrap

WS		[ \t\n\r]
SIGN		[+-]
DIG		[0-9]
DIGs		{DIG}+
CHAR		[a-zA-Z_]
DIGorCHAR	{DIG}|{CHAR}

INTs		{SIGN}?{DIGs}
FLOATs		{SIGN}?({DIGs}"."|"."{DIGs}|{DIGs}"."{DIGs})
IDs		{CHAR}{DIGorCHAR}*

%%

{WS}		{								}

"const"		{	yTRACE(CONST);			return CONST;		}
"int"		{	yTRACE(INT_TYPE); 		return INT_TYPE;	}
"float"		{	yTRACE(FLOAT_TYPE); 		return FLOAT_TYPE;	}
"bool"		{	yTRACE(BOOL_TYPE);		return BOOL_TYPE;	}
"vec2"		{	yTRACE(VEC2_TYPE);		return VEC2_TYPE;	}
"vec3"		{	yTRACE(VEC3_TYPE);		return VEC3_TYPE;	}
"vec4"		{	yTRACE(VEC4_TYPE);		return VEC4_TYPE;	}
"ivec2"		{	yTRACE(IVEC2_TYPE);		return IVEC2_TYPE;	}
"ivec3"		{	yTRACE(IVEC3_TYPE);		return IVEC3_TYPE;	}
"ivec4"		{	yTRACE(IVEC4_TYPE);		return IVEC4_TYPE;	}
"bvec2"		{	yTRACE(BVEC2_TYPE);		return BVEC2_TYPE;	}
"bvec3"		{	yTRACE(BVEC3_TYPE);		return BVEC3_TYPE;	}
"bvec4"		{	yTRACE(BVEC4_TYPE);		return BVEC4_TYPE;	}

"+"		{	yTRACE(PLUS_SIGN);		return PLUS_SIGN;	}
"-"		{	yTRACE(MINUS_SIGN);		return MINUS_SIGN;	}
"*"		{	yTRACE(MULT_SIGN);		return MULT_SIGN;	}
"/"		{	yTRACE(DIV_SIGN);		return DIV_SIGN;	}
"^"		{	yTRACE(EXP_SIGN);		return EXP_SIGN;	}
"!"		{	yTRACE(NOT_SIGN);		return NOT_SIGN;	}
"&"		{	yTRACE(BITAND_SIGN)		return BITAND_SIGN;	}
"|"		{	yTRACE(BITOR_SIGN)		return BITOR_SIGN;	}
"&&"		{	yTRACE(LOGICAND_SIGN);		return LOGICAND_SIGN;	}
"||"		{	yTRACE(LOGICOR_SIGN);		return LOGICOR_SIGN;	}
"=="		{	yTRACE(ISEQUAL_SIGN);		return ISEQUAL_SIGN;	}
"!="		{	yTRACE(NOTEQUAL_SIGN);		return NOTEQUAL_SIGN;	}
"<"		{	yTRACE(LT_SIGN);		return LT_SIGN;		}
"<="		{	yTRACE(LE_SIGN);		return LE_SIGN;		}
">"		{	yTRACE(GT_SIGN);		return GT_SIGN;		}
">="		{	yTRACE(GE_SIGN);		return GE_SIGN;		}
"="		{	yTRACE(ASSIGN_SIGN);		return ASSIGN_SIGN;	}

"true"		{	yTRACE(TRUE_EXP);		return TRUE_EXP;	}
"false"		{	yTRACE(FALSE_EXP);		return FALSE_EXP;	}

"{"		{	yTRACE(LB_BRA);			return LB_BRA;		}
"}"		{	yTRACE(RB_BRA);			return RB_BRA;		}
"["		{	yTRACE(LM_BRA);			return LM_BRA;		}
"]"		{	yTRACE(RM_BRA);			return RM_BRA;		}
"("		{	yTRACE(LS_BRA);			return LS_BRA;		}
")"		{	yTRACE(RS_BRA);			return RS_BRA;		}

";"		{	yTRACE(SEMICOLON);		return SEMICOLON;	}
","		{	yTRACE(COMMA);			return COMMA;		}
"if"		{	yTRACE(IF);			return IF;		}
"else"		{	yTRACE(ELSE);			return ELSE;		}
"for"		{	yTRACE(FOR);			return FOR;		}
"while"		{	yTRACE(WHILE);			return WHILE;		}
"do"		{	yTRACE(DO);			return DO;		}
"/*"        	{	processStarComment();		return STAR_COMMENT;	}
"//"        	{	processSlashComment();		return SLASH_COMMENT;	}
"#include"	{	yTRACE(INCLUDE);		return INCLUDE;		}
"#define"	{	yTRACE(DEFINE);			return DEFINE;		}

"lit"		{	processID(yytext, FUNCTION);	return FUNCTION;	}
"dp3"		{	processID(yytext, FUNCTION);	return FUNCTION;	}
"rsq"		{	processID(yytext, FUNCTION);	return FUNCTION;	}
"return"	{	yTRACE(RETURN);			return RETURN;		}

{IDs}		{	processID(yytext, IDENTIFIER);	return IDENTIFIER;	}
{INTs}		{	processLit(yytext, INT_EXP);	return INT_EXP;		}
{FLOATs}	{	processLit(yytext, FLOAT_EXP);	return FLOAT_EXP;	}

.           	{	yERROR("Unknown token");				}

%%

int processStarComment() {

	int c1      = 0;
	int c2      = yyinput();
	int curline = yyline;
	for(;;) {
        	if (c2 == EOF) {
                	fprintf(errorFile, "\nLEXICAL ERROR, LINE %d: Unmatched /*\n", curline);
                  	errorOccurred = TRUE;
                  	yyterminate();
                }
                if (c1 == '*' && c2 == '/')
                	break;
                c1 = c2;
                c2 = yyinput();
                if (c1 == '\n' && c2 != EOF)
                	yyline++;
	}
	return 0;
}

int processSlashComment() {

	int c1      = yyinput();
	for(;;) {
		if (c1 == '\n') {
                	yyline++;
			break;
		}
		c1 = yyinput();
	}
	return 0;
}

int processID(char *yytext, int tokenKind) {

	if(strlen(yytext) > MAX_IDENTIFIER)
		yERROR("identifier name exceeds 32 characters");
	yylval.str = (char *) malloc( sizeof(char) * (strlen(yytext)+1) );
	strcpy(yylval.str, yytext);
	yTRACE(tokenKind);
	return 0;
}

int processLit(char *yytext, int tokenKind) {

	if(tokenKind == INT_EXP) {;
		long int num = (long int) atof(yytext);
		if(num > INT_MAX || num < INT_MIN)
			yERROR("integer exceeds the range from -2147483648 to +2147483647");
		yylval.num_int = num;
		yTRACE(tokenKind);
	}
	else {
		double num = atof(yytext);
		if(num > FLT_MAX || num < -FLT_MAX)
			yERROR("float exceeds the range from -1e37 to +1e37");
		yylval.num_float = num;
		yTRACE(tokenKind);
	}
	return 0;
}
