%{
#ifndef defs_hpp
#include "defs.hpp"
#define defs_hpp
#endif
using namespace std;
#define YYSTYPE sysyAST*
%}

%token IF ELSE WHILE BREAK RETURN CONTINUE
%token EQ NEQ LT LE GT GE
%token ADD SUB MUL DIV MOD
%token NOT AND OR ASSIGN
%token INTEGER INT KEY VOID CONST

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%right ASSIGN
%left OR
%left AND
%left EQ NEQ
%left GT LT GE LE
%left ADD SUB
%left MUL DIV MOD
%right UMINUS UPLUS NOT

%%

Program : StmtSeq { $$ = new _PROGRAM($1); sysyRoot = $$; }
    ;
    
Str : KEY { $$ = new _STRING(_sysy_str); }
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
    | ADD Expr %prec UPLUS { $$ = $2; }
    | NOT Expr { $$ = new _NOT($2); }
    | INTEGER { $$ = new _INTEGER(_sysy_val); }
    | KEY { $$ = new _VAR(_sysy_str); }
    | FuncRet { $$ = $1; }
    | ArrayItem { $$ = $1; }
    ;


FuncRet : Str '(' CallList ')' { $$ = new _FUNC_CALL($1->getToken(), $3); }
    | Str '(' ')' { $$ = new _FUNC_CALL($1->getToken(), NULL); }
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
    ;
ParamList : INT Param ',' ParamList { $$ = new _PARAM_LIST($2, $4); }
    | INT Param { $$ = new _PARAM_LIST($2, NULL); }
    ;
Param : Str { $$ = new _PARAM_VAR($1->getToken(), NULL); }
    | Str '[' ']' { $$ = new _PARAM_ARR($1->getToken(), NULL, NULL); }
    | Str '[' ']' AddrList { $$ = new _PARAM_ARR($1->getToken(), $4, NULL); }
    ;

    
ConstDefList : ConstDef ',' ConstDefList { $$ = new _STMT_SEQ($1, $3); }
    | ConstDef { $$ = $1; }
    ;
DefList : Def ',' DefList { $$ = new _STMT_SEQ($1, $3); }
    | Def { $$ = $1; }
    ;


Stmt : IF '(' Expr ')' Stmt %prec LOWER_THAN_ELSE { $$ = new _IF($3, $5); }
    | IF '(' Expr ')' Stmt ELSE Stmt { $$ = new _IF_ELSE($3, $5, $7); }
    | WHILE '(' Expr ')' Stmt { $$ = new _WHILE($3, $5);}
    | RETURN ';' { $$ = new _RETURN_VOID(); }
    | RETURN Expr ';' { $$ = new _RETURN_EXPR($2); }
    | CONTINUE ';' { $$ = new _CONTINUE(); }
    | BREAK ';' { $$ = new _BREAK(); }
    | '{' StmtSeq '}' { $$ = new _BLOCK($2); }
    | Assign { $$ = $1; }
    | Func { $$ = $1; }
    | Decl { $$ = $1; }
    | ';' { $$ = new _DUMMY(); }
    | Expr ';' { $$ = $1; }
    ;

Assign : Expr ASSIGN Expr ';' { $$ = new _ASSIGN($1, $3); }
    ;

Func : INT Str '(' ParamList ')' Stmt { $$ = new _FUNC($2->getToken(), $4, $6, false); }
    | VOID Str '(' ParamList ')' Stmt { $$ = new _FUNC($2->getToken(), $4, $6, true); }
    | INT Str '(' ')' Stmt { $$ = new _FUNC($2->getToken(), NULL, $5, false); }
    | VOID Str '(' ')' Stmt { $$ = new _FUNC($2->getToken(), NULL, $5, true); }
    ;

Decl : INT DefList ';' { $$ = $2; }
    | CONST INT ConstDefList ';' { $$ = $3; }
    ;


Def : Str { $$ = new _DEF_VAR($1->getToken(), NULL); }
    | Str ASSIGN InitItem { $$ = new _DEF_VAR($1->getToken(), $3); }
    | Str AddrList { $$ = new _DEF_ARR($1->getToken(), $2, NULL); }
    | Str AddrList ASSIGN InitItem { $$ = new _DEF_ARR($1->getToken(), $2, $4); }
    ;
ConstDef : Str { $$ = new _DEF_CONST_VAR($1->getToken(), NULL); }
    | Str ASSIGN InitItem { $$ = new _DEF_CONST_VAR($1->getToken(), $3); }
    | Str AddrList { $$ = new _DEF_CONST_ARR($1->getToken(), $2, NULL); }
    | Str AddrList ASSIGN InitItem { $$ = new _DEF_CONST_ARR($1->getToken(), $2, $4); }
    ;

InitItem : '{' '}' { $$ = new _TREE_NODE( new _TREE_LEAF(NULL)); }
    | '{' Init '}' { $$ = new _TREE_NODE((_TREE*)$2); }
    | Expr { $$ = new _TREE_LEAF($1); }
    ;
Init : InitItem { $$ = $1; }
    | InitItem ',' Init { $$ = $1; ((_TREE*)$1)->sibling = (_TREE*)$3; }
    ;
%%
