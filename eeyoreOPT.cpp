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
vector<string> global_var_list {};
int n;
string def[maxlines], use1[maxlines], use2[maxlines];
bool reserved[maxlines];

vector<int> adj[maxlines];
queue<int> que;

void _analyse_reach_construct() {
    for (int i = 0; i < n; i++) adj[i].clear();
    for (int i = 0; i < n; i++) {
        if (nxt[i] && i + 1 < n) adj[i + 1].push_back(i);
        if (!jmp[i].empty()) adj[label[jmp[i]]].push_back(i);
    }
}
void _analyse_reach(string var_name) {
    /* var ought to be local */
    bitset<maxlines> &reach = regManager->vars[var_name]->active;
    reach.reset(); assert(que.empty());
    for (int i = 0; i < n; i++)
        if (use1[i] == var_name || use2[i] == var_name)
            que.push(i), reach[i] = true;
    while (!que.empty()) {
        int i = que.front(); que.pop();
        for (auto j: adj[i]) {
            if (def[j] != var_name) {
                if (!reach[j])
                    {reach[j] = true; que.push(j);}
            }
            else
                reserved[j] = true;
        }
    }
}

bool cmp(string var1, string var2) {
    return regManager->vars[var1]->active.count() < regManager->vars[var2]->active.count();
}
void _eFUNC::optimize() {
    seq = ((_eSEQ *) body)->seq;
    n = seq.size();
    assert(n < maxlines);
    _tFUNC *tfunc = new _tFUNC(func, arity);
    regManager->new_environ();

    /* ================ */
    /*  param decl      */
    /* ================ */
    var_list.clear();
    for (int i = 0; i < arity; i++)
        regManager->newLocal("p" + to_string(i), 4, true);
    for (auto decl: seq)
        if (decl->isdef()) { decl->localDecl(); var_list.push_back(decl->getName()); }
    tfunc->mem = regManager->stack_size >> 2;
    /* ======================== */
    /*  control-flow graph      */
    /* ======================== */
    memset(reserved, false, n); /* unnecessary to clear map-table label */
    for (int i = 0; i < n; i++) {
        jmp[i].clear(); nxt[i] = false;
        seq[i]->_analyse_cf(i);
    }
    /* ======================== */
    /*  def-use analysis        */
    /* ======================== */
    for (int i = 0; i < n; i++) {
        def[i].clear(); use1[i].clear(); use2[i].clear();
        seq[i]->_analyse_def_use(def[i], use1[i], use2[i]); 
    }
    /* ======================== */
    /*  reachability analysis   */
    /* ======================== */
    memset(reserved, false, n);
    _analyse_reach_construct();
    for (auto var_name: var_list)
        _analyse_reach(var_name);
    for (int i = 0; i < arity; i++)
        _analyse_reach("p" + to_string(i));
    for (int i = 0; i < n; i++)
    if (!reserved[i]) {
        if (def[i].empty()) {reserved[i] = true; continue; }
        Variable *var = regManager->vars[def[i]];
        if (var->isglobal()) {reserved[i] = true; continue; }
        /* global vars def-use condition is much more complicated */
        if (type[i] == FUNCRET) {
            seq[i] = new _eCALL(((_eFUNCRET *) seq[i])->func);
            reserved[i] = true;
            /* though the returned value is not used,
                function call could have side effects */
        }
    }

    /* ======================== */
    /*  register allocation     */
    /* ======================== */
    for (int i = 0; i < arity; i++)
        regManager->must_allocate("p" + to_string(i), "a" + to_string(i));
    /* param is always in register */
    sort(var_list.begin(), var_list.end(), cmp);
    for (auto var_name: var_list)
        regManager->try_allocate(var_name);

    /* optimization above */
    /* ================== */
    /* translate below    */
    
    regManager->callee_store();

    for (auto var_name: var_list)
        regManager->preload(var_name);
    if (func == "main") {
        for (auto var_name: global_var_list)
            regManager->preload(var_name);
    }
    
    /* unused definitions must be eliminated. */
    for (currentLine = 0; currentLine < n; currentLine++)
        if (reserved[currentLine] && !seq[currentLine]->isdef())
            seq[currentLine]->translate();

    /* function structure:
        func [arity] [mem] body */
    tfunc->body = new _tSEQ(tiggerStmtList);
    tiggerStmtList.clear();
    tiggerList.push_back(tfunc);
}

void _eSEQ::optimize() {
    for (auto decl: seq)
        if (decl->isdef()) decl->globalDecl();
    for (auto decl: seq)
        if (decl->isdef()) {
            regManager->try_allocate(decl->getName());
            global_var_list.push_back(decl->getName());
        }
    for (auto func: seq)
        if (!func->isdef()) func->translate();
    tiggerRoot = new _tSEQ(tiggerList);
}