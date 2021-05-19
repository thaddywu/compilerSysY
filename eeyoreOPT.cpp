#include <bits/stdc++.h>
#ifndef defs_hpp
#include "defs.hpp"
#define defs_hpp
#endif
using namespace std;

int currentLine;

void _eDEFVAR::_analyse_def_use(string &def, string &use1, string &use2) {}
void _eDEFARR::_analyse_def_use(string &def, string &use1, string &use2) {}
void _eDIRECT::_analyse_def_use(string &def, string &use1, string &use2)
    { def = a->getName(); if (!t->isnum()) use1 = t->getName(); }
void _eUNARY::_analyse_def_use(string &def, string &use1, string &use2)
    { def = a->getName(); if (!t->isnum()) use1 = t->getName(); }
void _eBINARY::_analyse_def_use(string &def, string &use1, string &use2)
    { def = a->getName(); if (!t1->isnum()) use1 = t1->getName(); if (!t2->isnum()) use2 = t2->getName(); }
void _eSEEK::_analyse_def_use(string &def, string &use1, string &use2)
    { def = a->getName(); if (!x->isnum()) use2 = x->getName();}
void _eSAVE::_analyse_def_use(string &def, string &use1, string &use2)
    { if (!x->isnum()) use1 = x->getName(); if (!t->isnum()) use2 = t->getName(); }
void _eFUNCRET::_analyse_def_use(string &def, string &use1, string &use2)
    { def = a->getName(); }
void _ePARAM::_analyse_def_use(string &def, string &use1, string &use2)
    { if (!t->isnum()) use1 = t->getName(); }
void _eIFGOTO::_analyse_def_use(string &def, string &use1, string &use2)
    { if (!t1->isnum()) use1 = t1->getName(); if (!t2->isnum()) use2 = t2->getName(); }
void _eGOTO::_analyse_def_use(string &def, string &use1, string &use2) {}
void _eLABEL::_analyse_def_use(string &def, string &use1, string &use2) {}
void _eRETVOID::_analyse_def_use(string &def, string &use1, string &use2) {}
void _eRET::_analyse_def_use(string &def, string &use1, string &use2)
    { if (!t->isnum()) use1 = t->getName(); }
void _eCALL::_analyse_def_use(string &def, string &use1, string &use2) {}

typedef enum {
    DEFVAR, DEFARR, DIRECT, UNARY, BINARY, SEEK, SAVE,
    FUNCRET, CALL, PARAM, IFGOTO, GOTO, LABEL, RET, RETVOID
}typeAST;
typeAST type[maxlines];
map<string, int> label;
bool nxt[maxlines]; string jmp[maxlines];
void _eDEFVAR::_analyse_cf(int line) { nxt[line] = true; type[line] = DEFVAR; }
void _eDEFARR::_analyse_cf(int line) { nxt[line] = true; type[line] = DEFARR; }
void _eDIRECT::_analyse_cf(int line) { nxt[line] = true; type[line] = DIRECT; }
void _eUNARY::_analyse_cf(int line) { nxt[line] = true; type[line] = UNARY; }
void _eBINARY::_analyse_cf(int line) { nxt[line] = true; type[line] = BINARY; }
void _eSEEK::_analyse_cf(int line) { nxt[line] = true; type[line] = SEEK; }
void _eSAVE::_analyse_cf(int line) { nxt[line] = true; type[line] = SAVE; }
void _eFUNCRET::_analyse_cf(int line) { nxt[line] = true; type[line] = FUNCRET; }
void _ePARAM::_analyse_cf(int line) { nxt[line] = true; type[line] = PARAM; }
void _eIFGOTO::_analyse_cf(int line) { nxt[line] = true; jmp[line] = l; type[line] = IFGOTO; }
void _eGOTO::_analyse_cf(int line)  { nxt[line] = false; jmp[line] = l; type[line] = GOTO; }
void _eLABEL::_analyse_cf(int line) { nxt[line] = true; label[l] = line; type[line] = LABEL; }
void _eRETVOID::_analyse_cf(int line) { nxt[line] = false; type[line] = RETVOID; }
void _eRET::_analyse_cf(int line) { nxt[line] = false; type[line] = RET; }
void _eCALL::_analyse_cf(int line) { nxt[line] = true; type[line] = CALL; }

vector<eeyoreAST *> seq;
vector<string> var_list; 
int n;
string def[maxlines], use1[maxlines], use2[maxlines];
bitset<maxlines> reach[maxlines];
bool reserved[maxlines];

bool visited[maxlines];
bool _analyse_reach(string &var, int pos, bitset<maxlines> &reach, int def_pos, bool def_start) {
    if (visited[pos]) return reach[pos];
    bool used = false; visited[pos] = true;
    if (!def_start) {
        if (use1[pos] == var) used = true;
        if (use2[pos] == var) used = true;
        if (def[pos] == var) return reach[pos] = used;
    }
    if (nxt[pos] && pos + 1 < n)
        used |= _analyse_reach(var, pos + 1, reach, def_pos, false);
    if (!jmp[pos].empty())
        used |= _analyse_reach(var, label[jmp[pos]], reach, def_pos, false);
    return reach[pos] = used;
}

bool cmp(string var1, string var2) {
    return regManager->vars[var1]->active.count() > regManager->vars[var2]->active.count();
}
void _eFUNC::optimize() {
    cerr << func << endl;
    seq = ((_eSEQ *) body)->seq;
    n = seq.size();
    assert(n < maxlines);
    _tFUNC *tfunc = new _tFUNC(func, arity);
    regManager->new_environ();

    /* ================ */
    /*  param decl      */
    /* ================ */
    cerr << "param decl" << endl;
    var_list.clear();
    for (int i = 0; i < arity; i++) {
        regManager->newLocal("p" + to_string(i), 4, true);
        // var_list.push_back("p" + to_string(i));
    }
    for (auto decl: seq)
        if (decl->isdef()) { decl->localDecl(); var_list.push_back(decl->getName()); }
    tfunc->mem = regManager->stack_size >> 2;
    /* ======================== */
    /*  control-flow graph      */
    /* ======================== */
    cerr << "control flow" << endl;
    memset(reserved, true, n); /* unnecessary to clear map-table label */
    for (int i = 0; i < n; i++) {
        jmp[i].clear(); nxt[i] = false;
        seq[i]->_analyse_cf(i);
    }
    /* ======================== */
    /*  def-use analysis        */
    /* ======================== */
    cerr << "def-use analysis" << endl;
    for (int i = 0; i < n; i++) {
        def[i].clear(); use1[i].clear(); use2[i].clear();
        seq[i]->_analyse_def_use(def[i], use1[i], use2[i]); 
    }
    /* ======================== */
    /*  reachability analysis   */
    /* ======================== */
    cerr << "reachability analysis" << endl;
    for (auto var_name: var_list) {
        Variable *var = regManager->vars[var_name];
        var->active.reset();
        memset(visited, false, n);
        _analyse_reach(var_name, 0, var->active, -1, true);
        /* -1 stands for declaration */
    }
    for (int i = 0; i < n; i++) {
        reach[i].reset();
        if (def[i].empty()) continue;
        if (regManager->isglobal(def[i])) continue;
        /* global vars def-use condition is much more complicated */

        memset(visited, false, n);
        if (!_analyse_reach(def[i], i, reach[i], i, true)) {
            if (type[i] == FUNCRET)
                seq[i] = new _eCALL(((_eFUNCRET *) seq[i])->func);
                /* though the returned value is not used, function call could have side effects */
            else
                reserved[i] = false;
        }
        Variable *var = regManager->vars[def[i]];
        var->active |= reach[i];
    }

    /* ======================== */
    /*  register allocation     */
    /* ======================== */
    cerr << "register allocation" << endl;
    for (int i = 0; i < arity; i++)
        regManager->must_allocate("p" + to_string(i), "a" + to_string(i));
    /* param is always in register */
    sort(var_list.begin(), var_list.end());
    for (auto var_name: var_list)
        regManager->try_allocate(var_name);

    /* optimization above */
    /* ================== */
    /* translate below    */
    cerr << "translate" << endl;
    
    regManager->callee_store();

    for (auto var_name: var_list)
        regManager->preload(var_name);
    cerr << "preload ends" << endl;
    for (currentLine = 0; currentLine < n; currentLine++)
        if (reserved[currentLine] && !seq[currentLine]->isdef())
            {cerr << currentLine << " " << type[currentLine] << endl; seq[currentLine]->translate();}
    cerr << "translate ends" << endl;
    
    /* debug  cout << func << " " << arity << " " << n << endl;
    for (int i = 0; i < n; i++)
        { printf("%3d:\t", i); seq[i]->Dump(); printf("... [%s] def:%s use1:%s use2:%s [nxt%d] [jmp:%s]\n", reserved[i] ? "true":"false", def[i].c_str(), use1[i].c_str(), use2[i].c_str(), (int)nxt[i], jmp[i].c_str()); }*/

    /* function structure:
        func [arity] [mem] body */
    tfunc->body = new _tSEQ(tiggerStmtList);
    tiggerStmtList.clear();
    tiggerList.push_back(tfunc);
}

void _eSEQ::optimize() {
    for (auto decl: seq)
        if (decl->isdef()) decl->globalDecl();
    for (auto func: seq)
        if (!func->isdef()) func->translate();
    tiggerRoot = new _tSEQ(tiggerList);
}