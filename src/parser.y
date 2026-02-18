%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "ir.h"

void yyerror(const char *s);
int yylex(void);
extern int yylineno;
extern char *yytext;

ASTNode *root = NULL; // global root used by main.c
static const char *current_file = NULL;

// Enhanced error reporting
void reportSyntaxError(const char *msg, const char *token);
%}

%code requires {
typedef struct ASTNode ASTNode; /* Forward declaration for yylval types in header */
}

%union {
    int num;
    char *id;
    ASTNode *node;
}

%token <num> NUMBER
%token <id> ID
%token FOR IF ELSE WHILE
%token ASSIGN SEMICOLON LPAREN RPAREN LBRACE RBRACE COMMA
%token LBRACKET RBRACKET
%token LT GT LE GE EQ NEQ
%token PLUSPLUS DOT
%error-verbose

%type <node> program stmt_list statement assignment loop condition expr opt_stmt_list call call_stmt
%type <node> for_init for_inc assign_nosemi inc_nosemi
%type <node> arg_list opt_args array_decl number_list method_call

%%
program
    : stmt_list                 { root = $1; }
    ;

stmt_list
    : statement                 { $$ = $1; }
    | stmt_list statement       { $$ = appendSequence($1, $2); }
    ;

statement
    : assignment                { $$ = $1; }
    | loop                      { $$ = $1; }
    | condition                 { $$ = $1; }
    | call_stmt                 { $$ = $1; }
    | array_decl                { $$ = $1; }
    | method_call SEMICOLON     { $$ = $1; }
    ;

assignment
    : ID ASSIGN expr SEMICOLON  { $$ = createAssignNode($1, $3); }
    ;

for_init
    : assign_nosemi             { $$ = $1; }
    ;

for_inc
    : assign_nosemi             { $$ = $1; }
    | inc_nosemi                { $$ = $1; }
    ;

assign_nosemi
    : ID ASSIGN expr            { $$ = createAssignNode($1, $3); }
    ;

inc_nosemi
    : ID PLUSPLUS               { $$ = createAssignNode($1, NULL); }
    ;

loop
    : FOR LPAREN for_init SEMICOLON expr SEMICOLON for_inc RPAREN LBRACE opt_stmt_list RBRACE
                                { $$ = createLoopNode($3, $5, $7, $10); }
    ;

condition
    : IF LPAREN expr RPAREN LBRACE opt_stmt_list RBRACE
                                { $$ = createIfNode($3, $6); }
    ;

opt_stmt_list
    : /* empty */               { $$ = NULL; }
    | stmt_list                 { $$ = $1; }
    ;

expr
    : ID                        { $$ = createValueNode($1); }
    | NUMBER                    { $$ = createNumberNode($1); }
    | ID LBRACKET expr RBRACKET { $$ = createValueNode($1); }
    | LPAREN expr RPAREN        { $$ = $2; }
    | expr GT expr              { $$ = $1; }
    | expr LT expr              { $$ = $1; }
    ;

call
    : method_call                { $$ = $1; }
    | ID LPAREN opt_args RPAREN { $$ = createAlgorithmCallNode($1, $3); }
    ;

method_call
    : ID DOT ID LPAREN opt_args RPAREN { 
        // Create method call: obj.method(args)
        char full_name[128];
        snprintf(full_name, sizeof(full_name), "%s.%s", $1, $3);
        $$ = createAlgorithmCallNode(full_name, $5);
      }
    ;

opt_args
    : /* empty */               { $$ = NULL; }
    | arg_list                  { $$ = $1; }
    ;

arg_list
    : expr                      { $$ = $1; }
    | arg_list COMMA expr       { $$ = appendSequence($1, $3); }
    ;

call_stmt
    : call SEMICOLON            { $$ = $1; }
    ;

array_decl
    : ID LBRACKET RBRACKET ASSIGN LBRACKET number_list RBRACKET SEMICOLON
                                { $$ = createArrayDeclNode($1, $6 ? $6->array_values : NULL, $6 ? $6->array_count : 0); }
    ;

number_list
    : NUMBER                    { ASTNode *node = (ASTNode *)calloc(1, sizeof(ASTNode)); 
                                  if (node) {
                                    node->array_values = (int *)malloc(sizeof(int)); 
                                    if (node->array_values) {
                                      node->array_values[0] = $1; 
                                      node->array_count = 1;
                                    }
                                  }
                                  $$ = node; }
    | number_list COMMA NUMBER { ASTNode *prev = (ASTNode*)$1;
                                  ASTNode *node = (ASTNode *)calloc(1, sizeof(ASTNode));
                                  if (node && prev) {
                                    node->array_count = prev->array_count + 1;
                                    node->array_values = (int *)malloc(node->array_count * sizeof(int));
                                    if (node->array_values) {
                                      for (int i = 0; i < prev->array_count; i++) {
                                        node->array_values[i] = prev->array_values[i];
                                      }
                                      node->array_values[node->array_count - 1] = $3;
                                      free(prev->array_values);
                                      free(prev);
                                    }
                                  } else if (node) {
                                    node->array_values = (int *)malloc(sizeof(int));
                                    if (node->array_values) {
                                      node->array_values[0] = $3;
                                      node->array_count = 1;
                                    }
                                  }
                                  $$ = node; }
    ;

%%
void yyerror(const char *s) { fprintf(stderr, "Error: %s\n", s); }


