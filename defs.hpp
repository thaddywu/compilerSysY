#include <bits/stdc++.h>
#ifndef eeyoreAST_hpp
#include "eeyoreAST.hpp"
#define eeyoreAST_hpp
#endif
#ifndef sysyAST_hpp
#include "sysyAST.hpp"
#define sysyAST_hpp
#endif
#ifndef sysyLUT_hpp
#include "sysyLUT.hpp"
#define sysyLUT_hpp
#endif

/* defined in sysy.l*/
extern int _sysy_val;
extern string _sysy_str;
extern int yylineno;
extern int yylex();
extern void yyerror(char *mss);

/* defined in defs.cpp */
extern void eeyoreDecl(eeyoreAST *x);
extern void eeyoreStmt(eeyoreAST *x);

extern TokenManager *tokenManager;
extern FuncManager *funcManager;
extern vector<sysyAST *> globalInitList;
extern vector<eeyoreAST *> eeyoreList;
extern vector<eeyoreAST *> eeyoreStmtList;
extern vector<eeyoreAST *> eeyoreDeclList;

extern sysyAST *sysyRoot;
extern eeyoreAST *eeyoreRoot;
void print(string x) ;
void printTab(string x) ;