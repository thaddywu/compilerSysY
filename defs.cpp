#ifndef defs_hpp
#include "defs.hpp"
#define defs_hpp
#endif
using namespace std;

TokenManager *tokenManager = new TokenManager();
FuncManager *funcManager = new FuncManager();
RegManager *regManager = new RegManager();
vector<sysyAST *> globalInitList{};

sysyAST *sysyRoot;
eeyoreAST *eeyoreRoot;
tiggerAST *tiggerRoot;

vector<eeyoreAST *> eeyoreList{};
vector<eeyoreAST *> eeyoreDeclList{};
vector<eeyoreAST *> eeyoreStmtList{};

vector<tiggerAST *> tiggerList{};
vector<tiggerAST *> tiggerStmtList{};

int STK = 0;
string t0;

int flatten(vector<int> &dim) {
    /* return flattened size */
    int ret = 1; for (auto x: dim) ret *= x; return ret;
}

void eeyoreStmt(eeyoreAST *x) { eeyoreStmtList.push_back(x); }
void eeyoreDecl(eeyoreAST *x) { eeyoreDeclList.push_back(x); }
void tiggerStmt(tiggerAST *x) { tiggerStmtList.push_back(x); }
void tiggerDecl(tiggerAST *x) { tiggerList.push_back(x); }
void print(string x) { cout << x << endl; }
void printTab(string x) { cout << "\t" + x << endl; }
bool isreg(string x) { return x[0] == 's' || x[0] == 't' || x[0] == 'a' || x == "x0"; }
bool islogicop(string x) { return x == "!=" || x == "==" || x == "<" || x == ">" || x == "<=" || x == ">="; }
int binary_result(int a, string op, int b) {
    if (op == "+") return a + b;
    if (op == "-") return a - b;
    if (op == "*") return a * b;
    if (op == "/") return a / b;
    if (op == "\%") return a % b;
    if (op == "<") return a < b;
    if (op == "<=") return a <= b;
    if (op == ">") return a > b;
    if (op == ">=") return a >= b;
    if (op == "||") return a || b;
    if (op == "&&") return a && b;
    if (op == "==") return a == b;
    if (op == "!=") return a != b;
    assert(false);
}
int unary_result(string op, int b) {
    if (op == "!") return !b;
    if (op == "-") return -b;
    assert(false);
}