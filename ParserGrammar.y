/* Author:      Nick Williams
   Created:     4/1/2019
*/
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "IOMngr.h"
#include "Scanner.h"

//extern int yylex(); /* The next token function. */
//extern char *yytext; /* The matched token text. */
//extern int yyleng; /* The token text length. */

void yyerror(char *s);

#define YYSTYPE char *

%}

// These token definitions populate y.tab.h
// Single character tokens are their own token numbers (e.g. scanner returns
// the value ',' for the comma token)
%token INT_TOK 1
%token CHR_TOK 2
%token ASSIGN_TOK 3
%token INTLIT_TOK 4
%token IDENT_TOK 5

%%
Prog    : IDENT_TOK '{' StmtSeq '}'             { };
StmtSeq : Stmt ';' StmtSeq                      { };
StmtSeq :                                       { };
Stmt    : Decl                                  { };
Stmt    : Assign                                { printf("\n");};
Decl    : Type IDLst                            { };
Type    : INT_TOK                               { };
Type    : CHR_TOK                               { };
IDLst   : IDENT_TOK MLst                        { };
MLst    : ',' IDLst                             { };
MLst    :                                       { };
Assign  : LHS ASSIGN_TOK Expr                   { printf("%s =",$1);};
LHS     : IDENT_TOK                             { $$ = strdup(yytext);};
Expr    : Expr AddOp Term                       { /*printf("Expr -> Expr AddOp Term"); char * expr = malloc(sizeof($1) + sizeof($2) + sizeof($3));
                                                    strcpy(expr, $1); strcat(expr, $3); strcat(expr,$2);
                                                    $$ = expr;*/
                                                    printf("%s ", $2);};
Expr    : Term                                  { /*printf("Expr -> term %s !", $1);char * fact = malloc(sizeof($1));
                                                    strcpy(fact, $1); $$ = fact;*/};
Term    : Term MultOp Factor                    { /*char * term = malloc(sizeof($1) + sizeof($2) + sizeof($3));
                                                    strcpy(term, $1); strcat(term, $3); strcat(term, $2);
                                                    $$ = term;*/
                                                    printf("%s ", $2);};
Term    : Factor                                { $$ = (YYSTYPE) "";};
Factor  : '(' Expr ')'                          { /*char * expr = malloc(sizeof($2));
                                                  strcpy(expr,$2); $$ = expr;*/};
Factor  : '-' Factor                            { /*char * minus = malloc(sizeof($2) +2);
                                                  strcpy(minus,$2); strcat(minus, "- "); $$ = minus;*/};
Factor  : INTLIT_TOK                            { char * num = malloc(sizeof(yytext) + 1);strcpy(num, yytext); strcat(num, " ");
                                                    //$$ = num;
                                                    printf("%s",num);};
Factor  : IDENT_TOK                             { char * id = malloc(sizeof(yytext)); strcpy(id, yytext);
                                                  //$$ = id;
                                                  printf("%s ", id);};
AddOp   : '+'                                   { $$ = "+";};
AddOp   : '-'                                   { $$ = "-";};
MultOp  : '*'                                   { $$ = "*";};
MultOp  : '/'                                   { $$ = "/";};

%%

void
yyerror(char *s) {
 // Called when parser encounters error. Write newline to stdout in case partial
 // postfix expression has been generated. Format error message and post to
 // current line. IOMngr will then echo error line along with error message.
 char msg[256];
 printf("\n");
 snprintf(msg,255,"error: \"%s\" yytext: \"%s\"",s,yytext);
 PostMessage(MakeSpanFromLength(tokenStartPosition,yyleng),msg);
}
