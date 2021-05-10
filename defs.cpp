#include "defs.hpp"
using namespace std;

TokenManager *tokenManager = new TokenManager();
FuncManager *funcManager = new FuncManager();
vector<nodeAST *> globalInitList{};
string stmtPrintBuffer;
int flatten(vector<int> &dim) {
    /* return flattened size */
    int ret = 1; for (auto x: dim) ret *= x; return ret;
}
void printDecl(string str) { cout << str << endl; }
void printStmt(string str) { stmtPrintBuffer += str + "\n"; }
void refreshStmt() { cout << stmtPrintBuffer; stmtPrintBuffer = ""; }