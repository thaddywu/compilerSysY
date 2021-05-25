#ifndef defs_hpp
#include "defs.hpp"
#define defs_hpp
#endif
using namespace std;

VarManager *varManager = new VarManager();
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
string t0 = "s8";

int flatten(vector<int> &shape) {
    /* return flattened size */
    int ret = 1; for (auto x: shape) ret *= x; return ret;
}

void eeyoreStmt(eeyoreAST *x) { eeyoreStmtList.push_back(x); }
void eeyoreDecl(eeyoreAST *x) { eeyoreDeclList.push_back(x); }
void tiggerStmt(tiggerAST *x) { tiggerStmtList.push_back(x); }
void tiggerDecl(tiggerAST *x) { tiggerList.push_back(x); }

void print(string x) { cout << x << endl; }
void printTab(string x) { cout << "\t" + x << endl; }

bool isreg(string x) { return x[0] == 's' || x[0] == 't' || x[0] == 'a' || x == "x0"; }
bool islogicop(string x) { return x == "!=" || x == "==" || x == "<" || x == ">" || x == "<=" || x == ">="; }
bool isint10(int x) { return x >= -512 && x < 512; }
bool isint12(int x) { return x >= -2048 && x < 2048; }

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
string neg_logicop(string op) {
    if (op == "==") return "!=";
    if (op == "!=") return "==";
    if (op == "<") return ">=";
    if (op == ">") return "<=";
    if (op == "<=") return ">";
    if (op == ">=") return "<";
    assert(false);
}