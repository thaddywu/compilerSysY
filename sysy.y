%{
#include <bits/stdc++.h>
#include "sysyAST.hpp"
#include "sysyLUT.hpp"
using namespace std;
#define YYSTYPE nodeAST*
#define maxSon 3

extern int _minic_val;
extern string _minic_str;
extern int yylineno;
extern int yylex();
extern void yyerror(char *mss);

extern TokenManager *tokenManager;
%}

%token IF ELSE WHILE BREAK RETURN CONTINUE
%token EQ NEQ LT LE GT GE
%token ADD SUB MUL DIV MOD
%token NOT AND OR ASSIGN
%token INTEGER INT KEY VOID CONST
%left ','
%right ASSIGN
%left OR
%left AND
%left GT LT GE LE EQ NEQ
%left ADD SUB
%left MUL DIV MOD
%right NOT
%nonassoc UMINUS

%%

All : Expr {printf("%d\n", $1->eval()); fflush(stdout);}
    | CONST INT Str ASSIGN ConstInit ";" { fflush(stdout); }
    ;
    
Str : KEY { $$ = new _STRING(_minic_str); }
    ;

Expr : Expr ADD Expr { $$ = new _ADD($1, $3); }
    | Expr SUB Expr { $$ = new _SUB($1, $3); }
    | Expr MUL Expr { $$ = new _MUL($1, $3); }
    | Expr DIV Expr { $$ = new _DIV($1, $3); }
    | Expr MOD Expr { $$ = new _MOD($1, $3); }
    | Expr AND Expr { $$ = new _AND($1, $3); }
    | Expr OR Expr { $$ = new _OR($1, $3); }
    | Expr LT Expr { $$ = new _LT($1, $3); }
    | Expr GT Expr { $$ = new _GT($1, $3); }
    | Expr LE Expr { $$ = new _LE($1, $3); }
    | Expr GE Expr { $$ = new _GE($1, $3); }
    | Expr NEQ Expr { $$ = new _NEQ($1, $3); }
    | Expr EQ Expr { $$ = new _EQ($1, $3); }
    | '(' Expr ')' { $$ = $2; }
    | SUB Expr %prec UMINUS { $$ = new _NEG($2); }
    | NOT Expr { $$ = new _NOT($2); }
    | INTEGER { $$ = new _INTEGER(_minic_val); }
    ;

    | KEY { $$ = new _VAR(_minic_str); }
    | FuncCall { $$ = $1; }
    | ArrayItem { $$ = $1; }
    ;


FuncCall : Str '(' CallList ')' { $$ = new _FUNC_CALL($1->getToken(), $3); }
    ;
ArrayItem : Str AddrList { $$ = new _ARRAY_ITEM($1->getToken(), $2); }
    ;

StmtSeq : Stmt StmtSeq { $$ = new _STMT_SEQ($1, $2); }
    | { $$ = NULL; }
    ;
AddrList : '[' Expr ']' AddrList { $$ = new _ADDR_LIST($2, $4); }
    | '[' Expr ']' { $$ = new _ADDR_LIST($2, NULL); }
    ;
CallList : Expr ',' CallList { $$ = new _CALL_LIST($1, $3); }
    | Expr { $$ = new _CALL_LIST($1, NULL); }
    | { $$ = NULL; }
    ;
ParamList : INT Expr ',' ParamList { $$ = new _PARAM_LIST($2, $4); }
    | INT Expr { $$ = new _PARAM_LIST($2, NULL); }
    | { $$ = NULL; }
    ;
ConstDefList : ConstDef ',' ConstDefList { $$ = new _STMT_SEQ($1, $3); }
    | ConstDef { $$ = $1; }
    ;
DefList : Def ',' DefList { $$ = new _STMT_SEQ($1, $3); }
    | Def { $$ = $1; }
    ;


Stmt : IF '(' Expr ')' Stmt { $$ = new _IF($3, $5); }
    | IF '(' Expr ')' Stmt ELSE Stmt { $$ = new _IF_THEN($3, $5, $7); }
    | WHILE '(' Expr ')' Stmt { $$ = new _WHILE($3, $5);}
    | RETURN ';' { $$ = new _RETURN_VOID(); }
    | RETURN Expr ';' { $$ = new _RETURN_EXPR($2); }
    | CONTINUE ';' { $$ = new _CONTINUE(); }
    | '{' StmtSeq '}' { $$ = $2; }
    | Assign { $$ = $1; }
    | Func { $$ = $1; }
    | Decl { $$ = $1; }
    ;

Assign : Expr ASSIGN Expr { $$ = new _ASSIGN($1, $3); }
    ;

Func : INT Str '(' ParamList ')' '{' StmtSeq '}' { $$ = new _FUNC($2->getToken(), $4, $7); }
    | VOID Str '(' ParamList ')' '{' StmtSeq '}' { $$ = new _FUNC($2->getToken(), $4, $7); }
    ;

Decl : INT DefList ';' { $$ = $2; }
    | CONST INT ConstDefList ';' { $$ = $3; }
    ;


Def : Str { $$ = new _DEF_VAR($1->getToken(), NULL); }
    | Str ASSIGN Expr { $$ = new _DEF_VAR($1->getToken(), $3); }
    | Str AddrList { $$ = new _DEF_ARR($1->getToken(), $2, NULL); }
    | Str AddrList ASSIGN ConstInit { $$ = new _DEF_ARR($1->getToken(), $2, $4); }
    ;
ConstDef : Str { $$ = new _DEF_CONST_VAR($1->getToken(), NULL); }
    | Str ASSIGN ASSIGN Expr { $$ = new _DEF_CONST_VAR($1->getToken(), $3); }
    | Str AddrList { $$ = new _DEF_CONST_ARR($1->getToken(), $2, NULL); }
    | Str AddrList ASSIGN ConstInit { $$ = new _DEF_CONST_ARR($1->getToken(), $2, $4); }
    ;

ConstInitItem : '{' '}' { $$ = new _TREE_NODE( new _TREE_LEAF(NULL)); }
    | '{' ConstInit '}' { $$ = new _TREE_NODE((_TREE*)$2); }
    | Expr { $$ = new _TREE_LEAF($1); }
    ;
ConstInit : ConstInitItem { $$ = $1; }
    | ConstInitItem ',' ConstInit { $$ = $1; ((_TREE_NODE*)$1)->sibling = (_TREE*)$2; }
    ;
%%

int main()
{
    tokenManager = new TokenManager();
    yyparse();
    return 0;
}