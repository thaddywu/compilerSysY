#include <bits/stdc++.h>
#ifndef sysyAST_hpp
#include "sysyAST.hpp"
#define sysyAST_hpp
#endif
#ifndef sysyLUT_hpp
#include "sysyLUT.hpp"
#define sysyLUT_hpp
#endif
#ifndef eeyoreAST_hpp
#include "eeyoreAST.hpp"
#define eeyoreAST_hpp
#endif
#ifndef eeyoreREG_hpp
#include "eeyoreREG.hpp"
#define eeyoreREG_hpp
#endif
#ifndef tiggerAST_hpp
#include "tiggerAST.hpp"
#define tiggerAST_hpp
#endif

/* defined in sysy.l*/
extern int _sysy_val;
extern string _sysy_str;
extern int yylineno;
extern int yylex();
extern void yyerror(char *mss);

extern sysyAST *sysyRoot;
extern eeyoreAST *eeyoreRoot;
extern tiggerAST *tiggerRoot;

/* tools for sysY->eeyore */
extern VarManager *varManager;
extern FuncManager *funcManager;
extern vector<sysyAST *> globalInitList;
extern vector<eeyoreAST *> eeyoreList; /* global decls with functions */
extern vector<eeyoreAST *> eeyoreStmtList; /* statement list in function */
extern vector<eeyoreAST *> eeyoreDeclList; /* decls */

extern void eeyoreDecl(eeyoreAST *x);
extern void eeyoreStmt(eeyoreAST *x);

/* tools for eeyore->tigger */
extern RegManager *regManager;
extern vector<tiggerAST *> tiggerList;
extern vector<tiggerAST *> tiggerStmtList;

extern bool isreg(string x) ;
extern bool islogicop(string x) ;
extern void tiggerDecl(tiggerAST *x);
extern void tiggerStmt(tiggerAST *x);

/* handy tools */
extern void print(string x) ;
extern void printTab(string x) ;

extern int unary_result(string op, int a);
extern int binary_result(int a, string op, int b);
extern string neg_logicop(string op);

extern bool isint10(int x) ;
extern bool isint12(int x) ;

extern bool is2power(int x) ;
extern int get2log(int x) ;

extern int STK ;