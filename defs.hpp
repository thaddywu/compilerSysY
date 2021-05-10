#include <bits/stdc++.h>
#include "sysyAST.hpp"
#include "sysyLUT.hpp"

/* defined in sysy.l*/
extern int _sysy_val;
extern string _sysy_str;
extern int yylineno;
int yylex();
void yyerror(char *mss);

/* defined in defs.cpp */
extern string StmtPrintBuffer;
void printDecl(string str) ;
void printStmt(string str) ;
void refreshStmt() ;

extern TokenManager *tokenManager;
extern FuncManager *funcManager;
extern vector<nodeAST *> globalInitList;