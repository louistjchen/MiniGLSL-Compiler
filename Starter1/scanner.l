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


#define YY_USER_INIT { yyin = inputFile; }
#define	yyinput      input
#define yTRACE(x)    { if (traceScanner) fprintf(traceFile, "TOKEN %3d : %s\n", x, yytext); }
#define yERROR(x)    { fprintf(errorFile, "\nLEXICAL ERROR, LINE %d: %s\n", yyline, x); errorOccurred = TRUE; yyterminate(); }

int CheckInt(void);
int yyline = 1;
int processStarComment();
int processSlashComment();
int processID(char *yytext, int tokenKind);


%}
%option noyywrap

WS		[ \t]
DIGIT		[0-9]
DIGITS		{DIGIT}+
INTS		{DIGITS}
FLOATS		{DIGITS}.

%%

"int"		{	yTRACE(INT_TYPE); 	return INT_TYPE;	}
"float"		{	yTRACE(FLOAT_TYPE); 	return FLOAT_TYPE;	}
"bool"		{	yTRACE(BOOL_TYPE);	return BOOL_TYPE;	}
"vec2"		{	yTRACE(VEC2_TYPE);	return VEC2_TYPE;	}
"vec3"		{	yTRACE(VEC3_TYPE);	return VEC3_TYPE;	}
"vec4"		{	yTRACE(VEC4_TYPE);	return VEC4_TYPE;	}
"ivec2"		{	yTRACE(IVEC2_TYPE);	return IVEC2_TYPE;	}
"ivec3"		{	yTRACE(IVEC3_TYPE);	return IVEC3_TYPE;	}
"ivec4"		{	yTRACE(IVEC4_TYPE);	return IVEC4_TYPE;	}
"bvec2"		{	yTRACE(BVEC2_TYPE);	return BVEC2_TYPE;	}
"bvec3"		{	yTRACE(BVEC3_TYPE);	return BVEC3_TYPE;	}
"bvec4"		{	yTRACE(BVEC4_TYPE);	return BVEC4_TYPE;	}
	
"+"		{	yTRACE(PLUS_SIGN);	return PLUS_SIGN;	}
"-"		{	yTRACE(MINUS_SIGN);	return MINUS_SIGN;	}
"*"		{	yTRACE(MULT_SIGN);	return MULT_SIGN;	}
"/"		{	yTRACE(DIV_SIGN);	return DIV_SIGN;	}
"^"		{	yTRACE(EXP_SIGN);	return EXP_SIGN;	}
"!"		{	yTRACE(NOT_SIGN);	return NOT_SIGN;	}
"&"		{	yTRACE(BITAND_SIGN)	return BITAND_SIGN;	}
"|"		{	yTRACE(BITOR_SIGN)	return BITOR_SIGN;	}
"&&"		{	yTRACE(LOGICAND_SIGN);	return LOGICAND_SIGN;	}
"||"		{	yTRACE(LOGICOR_SIGN);	return LOGICOR_SIGN;	}
"=="		{	yTRACE(ISEQUAL_SIGN);	return ISEQUAL_SIGN;	}
"!="		{	yTRACE(NOTEQUAL_SIGN);	return NOTEQUAL_SIGN;	}
"<"		{	yTRACE(LT_SIGN);	return LT_SIGN;		}
"<="		{	yTRACE(LE_SIGN);	return LE_SIGN;		}
">"		{	yTRACE(GT_SIGN);	return GT_SIGN;		}
">="		{	yTRACE(GE_SIGN);	return GE_SIGN;		}
"="		{	yTRACE(ASSIGN_SIGN);	return ASSIGN_SIGN;	}

"true"		{	yTRACE(TRUE_EXP);	return TRUE_EXP;	}
"false"		{	yTRACE(FALSE_EXP);	return FALSE_EXP;	}

"{"		{	yTRACE(LB_BRA);		return LB_BRA;		}
"}"		{	yTRACE(RB_BRA);		return RB_BRA;		}
"["		{	yTRACE(LM_BRA);		return LM_BRA;		}
"]"		{	yTRACE(RM_BRA);		return RM_BRA;		}
"("		{	yTRACE(LS_BRA);		return LS_BRA;		}
")"		{	yTRACE(RS_BRA);		return RS_BRA;		}

";"		{	yTRACE(SEMICOLON);	return SEMICOLON;	}
","
"/*"        	{	processStarComment();	return STAR_COMMENT;	}
"//"        	{	processSlashComment();	return SLASH_COMMENT;	}

"lit"		{	processID(yytext, FUNCTION);	return FUNCTION;}
"dp3"		{	processID(yytext, FUNCTION);	return FUNCTION;}
"rsq"		{	processID(yytext, FUNCTION);	return FUNCTION;}

.           	{	yERROR("Unknown token");			}

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

	yylval.str = (char *) malloc( sizeof(char) * (strlen(yytext)+1) );
	strcpy(yylval.str, yytext);
	yTRACE(tokenKind);
	return 0;
}
