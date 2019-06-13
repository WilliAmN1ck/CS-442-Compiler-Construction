Letter	[A-Za-z]
Digit   [0-9]

%{
#include "ScanTokens.h"
#include "IOMngr.h"

#define YY_INPUT(buf,result,max_size) \
	{ int c = GetSourceChar(); \
		result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
	}

// Set to true to log each token returned to stderr
bool shouldLog = true;

// The column of the returned token's first char
unsigned int tokenStartPosition;

// Return token number
// offset = 1 for tokens that include the last char read
// offset = 0 for tokens that do not include the last char read
#define RETURN(tokenNumber,offset) \
  { if (shouldLog) fprintf(stderr,"Token#=%d, Length=%zu, Text=\"%s\"\n", \
                          tokenNumber,(unsigned long)yyleng,yytext); \
    tokenStartPosition = GetLastPosition() - yyleng + offset; \
    return(tokenNumber); \
	}

//var for comments
/*vars for helping figure out if in a comment or not*/

unsigned int lineComment = 0;
unsigned int blockComment = 0;
#define IN_COMMENT \
	{ (return lineComment == 0 && blockComment == 0) \
	}\
%}

operators							[\+|\-]
asterisk							[*]
slash									[/]

%%
\@INIT                          		{ if (lineComment == 0 && blockComment == 0) RETURN(INIT_TOK,1); }
\@DUMP                          		{ if (lineComment == 0 && blockComment == 0) RETURN(DUMP_TOK,1); }
{Letter}({Letter}|{Digit})*					{ if (lineComment == 0 && blockComment == 0) RETURN(IDENT_TOK,0); }
(\+|\-)?{Digit}+\.{Digit}+					{ if (lineComment == 0 && blockComment == 0) RETURN(FLOAT_TOK,0);}
(\+|\-)?{Digit}({Digit})*										{ if (lineComment == 0 && blockComment == 0) RETURN(INT_TOK,0);}
[=]																	{ if (lineComment == 0 && blockComment == 0) RETURN(EQUAL_TOK,0);}
[ \t]                          		 { }
[\n]                           		 { lineComment = 0; }
{slash}{asterisk}										{ blockComment++; }
{asterisk}{slash}										{ blockComment--; }
{slash}{slash}											{ lineComment++; }
.                             		  { }
%%
