/***********************************************************************
 * **YOUR GROUP INFO SHOULD GO HERE**
 * Louis Chen - 1000303502
 * Juntu Chen - 1000659799
 *
 * compiler467.c
 *
 * This is the main driver program for the CSC467F course project
 * compiler.  It handles compiler command line options and
 * initialization, calls the parser to run the compilation and then
 * invokes the pseudo machine interpreter to execute the compiled
 * program.
 **********************************************************************/

/***********************************************************************
 * The compiler has the following parts:
 * global variables     globalvars.c common.h languageDef.h
 * scanner module       scanner.c
 * parser module        parser.c     parser.tab.h
 * abstract syntax tree ast.c        ast.h
 * symbol table         symbol.c     symbol.h
 * semantics analysis   semantic.c   semantic.h
 * code generator       codegen.c    codegen.h
 **********************************************************************/
#include "common.h"

/* Phases 3,4: Uncomment following includes as needed */
#include "ast.h"
#include "symbol.h"
#include "semantic.h"
#include "codegen.h"

/***********************************************************************
 * Default values for various files. Note assumption that default files
 * are not closed at the end of compilation.
 **********************************************************************/
#define DEFAULT_INPUT_FILE     stdin
#define DEFAULT_OUTPUT_FILE    stdout
#define DEFAULT_ERROR_FILE     stderr
#define DEFAULT_DUMP_FILE      stdout
#define DEFAULT_TRACE_FILE     stdout
#define DEFAULT_RUN_INPUT_FILE stdin

void  getOpts   (int numargs, char **argstr);
FILE *fileOpen  (char *fileName, char *fileMode, FILE *defaultFile);
void  sourceDump(void);

/* Phase 1: Scanner Interface. For phase 2 and after these declarations
 * are removed */
/*
extern int   yylex(void);
extern char *yytext;
extern int   yyline;
 */

/* Phase 2: Parser Interface. Merely uncomment the following line */
extern int yyparse(void);

/***********************************************************************
 * Main program for the Compiler
 **********************************************************************/
int main (int argc, char *argv[]) {

  getOpts (argc, argv); /* Set up and apply command line options */

/***********************************************************************
 * Compiler Initialization.
 *
 * If explicit initialization of any phase of the compilation is needed,
 * calls to initialization routines in the applicable modules are placed
 * here.
 **********************************************************************/
  errorOccurred = FALSE;

/***********************************************************************
 * Start the Compilation
 **********************************************************************/
  if (dumpSource)
    sourceDump();

/* Phase 1: Scanner. In phase 2 and after the following code should be
 * removed */
/*
  while (yylex())
    if (errorOccurred)
      break;
 */

  /* We initialize global variables listed in globalvars.c, extern-ed in common.h.
   * These contain scope variables used for symbol table regeration, ast traversal,
   * and some dummys. Please refer to "symbol.c" for details. */
  initGlobalVars();

/* Phase 2: Parser -- should allocate an AST, storing the reference in the
 * global variable "ast", and build the AST there. */
  if (1 == yyparse())
    return 0; // parse failed

  /* After AST is built, we implemented another AST traversal function called
   * configScopeParent(node *ast, node *parentAst) to update our AST, so that
   * every node has a backtrack pointer to its parent node. This is very useful
   * as we use parent information in both semantic check and code generation
   * stages. Please refer to "ast.c" for details. */
  configScopeParent(ast, NULL);

  /* According to the specification of handout, there are several predefined
   * variables, such as gl_Color, that cannot be re-declared. Therefore, we
   * insert them into our symbol table corresponding information. Please refer
   * to "symbol.c" for details. */
  insertPredefVars();

  /* After inserting predefined variables into symbol table, we now start to
   * traverse AST and build up our symbol table, which will be containing
   * the scope, type, variable name, etc information on all variables.
   * Please refer to "symbol.c" for details. */
  symbol_table(ast);

  /* After symbol table is built, we initialize our register list, which will
   * contain register name/info for all predefined variables, in a linked-list
   * manner. Please refer to "codegen.c" for details. */
  initRegList();

  /* Dump symbol table if "-Ds" is included in the command. We could print this
   * after semantic checking, but this is for symbol table debugging. Please
   * refer to "symbol.c" for detailes. */
  if (dumpSymbols)
    print_symbol_table(Head);

  /* Now that we have AST and symbol table built, we can start running semantic
   * checking for the input code in a post-order traversal. Please refer to
   * "semantic.c" for details. */
  if (semantic_check(ast) == -1) {
    printf("Semantic check failed\n");
    errorOccurred = TRUE;
  }

/* Phase 3: Call the AST dumping routine if requested */
  if (dumpAST)
    if(errorOccurred)
	    printf("Since semantic check fails. Don't print ast tree.\n");
    else
      ast_print(ast);

/* Phase 4: Add code to call the code generation routine */
/* TODO: call your code generation routine here */
  if (errorOccurred)
    fprintf(outputFile,"Failed to compile\n");
  else 
   genCode(ast);
  
/***********************************************************************
 * Post Compilation Cleanup
 **********************************************************************/

/* Make calls to any cleanup or finalization routines here. */

  /* Regardless whether the input code is compiled successfully or not, we
   * need to free all dynamically allocated memories. These include AST
   * tree, symbol table linked-list, and register linked-list. Please
   * refer to "ast.c", "symbol.c", "codegen.c" for details. */
  ast_free(ast);
  freeSymbolTable();
  freeRegList();

  /* Clean up files if necessary */
  if (inputFile != DEFAULT_INPUT_FILE)
    fclose (inputFile);
  if (errorFile != DEFAULT_ERROR_FILE)
    fclose (errorFile);
  if (dumpFile != DEFAULT_DUMP_FILE)
    fclose (dumpFile);
  if (traceFile != DEFAULT_TRACE_FILE)
    fclose (traceFile);
  if (outputFile != DEFAULT_OUTPUT_FILE)
    fclose (outputFile);
  if (runInputFile != DEFAULT_RUN_INPUT_FILE)
    fclose (runInputFile);

  return 0;
}

/***********************************************************************
Internal Subroutines.
***********************************************************************/

/***********************************************************************
Subroutines for reading command line input and initializing IO files.
***********************************************************************/
void getOpts (int numargs, char **argstr) {
  char *optarg;
  char *subarg;
  int   i;
  char  optch;

  /* Initialize global variables to default values */
  inputFile         = DEFAULT_INPUT_FILE;
  outputFile        = DEFAULT_OUTPUT_FILE;
  errorFile         = DEFAULT_ERROR_FILE;
  dumpFile          = DEFAULT_DUMP_FILE;
  traceFile         = DEFAULT_TRACE_FILE;
  runInputFile      = DEFAULT_RUN_INPUT_FILE;

  /* Initialize control flags */
  suppressExecution = FALSE;

  traceScanner      = FALSE;
  traceParser       = FALSE;
  traceExecution    = FALSE;

  dumpSource        = FALSE;
  dumpAST           = FALSE;
  dumpSymbols       = FALSE;
  dumpInstructions  = FALSE;

  /* Process command line input */
  for (i=1; i<numargs; i++) {
    optarg = argstr[i];
    if (optarg[0] == '-') { /* Compiler option */
      subarg = optarg + 2;
      switch (optarg[1]) {
        case 'D': /* Dump options -Dasxy */
          optch = *(subarg++);
          while (optch) {
            switch (optch) {
              case 'a': dumpAST          = TRUE; break;
              case 's': dumpSource       = TRUE; break;
              case 'x': dumpInstructions = TRUE; break;
              case 'y': dumpSymbols      = TRUE; break;
              default: fprintf(errorFile, "Invalid dump option %c ignored\n", optch); break ;
            }
            optch = *(subarg++);
          }
          break;
        case 'T': /* Trace options -Tnpx */
          optch = *(subarg++);
          while (optch) {
            switch (optch) {
              case 'n': traceScanner   = TRUE; break;
              case 'p': traceParser    = TRUE; break;
              case 'x': traceExecution = TRUE; break;
              default: fprintf(errorFile, "Invalid trace option %c ignored\n", optch); break;
            }
            optch = *(subarg++);
          }
          break;
        case 'O': /* Alternative output file */
          printf("Alternative output file\n");
          if (optarg[2] == 0) {
            i += 1;
            outputFile = fileOpen (argstr[i], "w", DEFAULT_OUTPUT_FILE);
          printf("%s\n",argstr[i]);
          } else
            outputFile = fileOpen (&optarg[2], "w", DEFAULT_OUTPUT_FILE);
          printf("%s\n",&optarg[2]);
          break;
        case 'E': /* Alternative error message file */
          if (optarg[2] == 0) {
            i += 1;
            errorFile = fileOpen (argstr[i], "w", DEFAULT_OUTPUT_FILE);
          } else
            errorFile = fileOpen (&optarg[2], "w", DEFAULT_ERROR_FILE);
          break;
        case 'R': /* Alternative sink for traces */
          if (optarg[2] == 0) {
            i += 1;
            traceFile = fileOpen (argstr[i], "w", DEFAULT_TRACE_FILE);
          } else
            traceFile = fileOpen (&optarg[2], "w", DEFAULT_TRACE_FILE);
          break;
        case 'U': /* Alternative sink for dumps */
          if (optarg[2] == 0) {
            i += 1;
            dumpFile = fileOpen (argstr[i], "w", DEFAULT_DUMP_FILE);
          } else
            dumpFile = fileOpen (&optarg[2], "w", DEFAULT_DUMP_FILE);
          break;
        case 'I': /* Alternative input during execution */
          if (optarg[2] == 0) {
            i += 1;
            runInputFile = fileOpen (argstr[i], "r", DEFAULT_RUN_INPUT_FILE);
          } else
            runInputFile = fileOpen (&optarg[2], "r", DEFAULT_RUN_INPUT_FILE);
          break;
        case 'X': /* supress execution flag */
          suppressExecution = TRUE;
          break;
        default: /* Anything else */
          fprintf(stderr,"Unknown option character %c (ignored)\n", optch);
          break;
      }
    } else /* Source file */
      inputFile = fileOpen(optarg , "r", DEFAULT_INPUT_FILE);
  }
}

/***********************************************************************
 * Utility for opening files 
 **********************************************************************/
FILE *fileOpen (char *fileName, char *fileMode, FILE *defaultFile) {
  FILE * fTemp;

  if ((fTemp = fopen (fileName, fileMode)) != NULL)
    return fTemp;
  else {
    fprintf (errorFile, "Unable to open file %s\n", fileName);
    return defaultFile;
  }
}

/***********************************************************************
 * Dump source file, with line numbers.
 **********************************************************************/
void sourceDump (void) {
  char srcbuf[MAX_TEXT];
  int i = 0;

  while (fgets(srcbuf, MAX_TEXT, inputFile)) {
    i += 1;
    fprintf(dumpFile, "%3d: %s", i, srcbuf);
  }
  rewind(inputFile);
}

