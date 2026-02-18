%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"

// yylval is provided by bison
extern int yylineno;

// Track column position for better error reporting
static int column = 1;

// Error reporting
static void reportLexicalError(const char *token, int line, int col);
%}

%%
"for"                   { return FOR; }
"if"                    { return IF; }
"else"                  { return ELSE; }
"while"                 { return WHILE; }
"."                     { return DOT; }

"=="                    { return EQ; }
"!="                    { return NEQ; }
"<="                    { return LE; }
">="                    { return GE; }
"<"                     { return LT; }
">"                     { return GT; }
"++"                    { return PLUSPLUS; }
"="                     { return ASSIGN; }
";"                     { return SEMICOLON; }
"("                     { return LPAREN; }
")"                     { return RPAREN; }
"{"                     { return LBRACE; }
"}"                     { return RBRACE; }
","                     { return COMMA; }
"["                     { return LBRACKET; }
"]"                     { return RBRACKET; }

[0-9]+                  { 
                            yylval.num = atoi(yytext); 
                            column += yyleng;
                            return NUMBER; 
                        }
[a-zA-Z_][a-zA-Z0-9_]*  { 
                            yylval.id = _strdup(yytext); 
                            column += yyleng;
                            return ID; 
                        }

\n                      { 
                            yylineno++; 
                            column = 1; 
                        }
[ \t\r]+                { column += yyleng; }
.                       { 
                            reportLexicalError(yytext, yylineno, column);
                            column += yyleng;
                            return 0; // Return 0 to signal error to parser
                        }
%%
static void reportLexicalError(const char *token, int line, int col) {
    fprintf(stderr, "Lexical Error: Invalid character '%s' at line %d, column %d\n", token, line, col);
    fprintf(stderr, "  Unexpected character encountered. Valid characters include: letters, numbers, operators, and punctuation.\n");
}

int yywrap(void) { return 1; }


