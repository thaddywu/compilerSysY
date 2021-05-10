#ifndef defs_hpp
#include "defs.hpp"
#define defs_hpp
#endif
using namespace std;

TokenManager *tokenManager = new TokenManager();
FuncManager *funcManager = new FuncManager();
vector<sysyAST *> globalInitList{};

sysyAST *sysyRoot;
eeyoreAST *eeyoreRoot;
vector<eeyoreAST *> eeyoreList{};
vector<eeyoreAST *> eeyoreDeclList{};
vector<eeyoreAST *> eeyoreStmtList{};
int flatten(vector<int> &dim) {
    /* return flattened size */
    int ret = 1; for (auto x: dim) ret *= x; return ret;
}

void eeyoreStmt(eeyoreAST *x) { eeyoreStmtList.push_back(x); }
void eeyoreDecl(eeyoreAST *x) { eeyoreDeclList.push_back(x); }
void print(string x) { cout << x << endl; }
void printTab(string x) { cout << "\t" + x << endl; }