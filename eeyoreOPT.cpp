#include <bits/stdc++.h>
#ifndef defs_hpp
#include "defs.hpp"
#define defs_hpp
#endif
using namespace std;

int currentLine;

string def[maxlines], use1[maxlines], use2 [maxlines], use3[maxlines];
void _eDEFVAR::_analyse_def_use(int line) {}
void _eDEFARR::_analyse_def_use(int line) {}
void _eDIRECT::_analyse_def_use(int line)
    { def[line] = a->getName(); use1[line] = t->getName(); }
void _eUNARY::_analyse_def_use(int line)
    { def[line] = a->getName(); use1[line] = t->getName(); }
void _eBINARY::_analyse_def_use(int line)
    { def[line] = a->getName(); use1[line] = t1->getName(); use2[line] = t2->getName(); }
void _eSEEK::_analyse_def_use(int line)
    { def[line] = a->getName(); use2[line] = x->getName(); use3[line] = t;}
void _eSAVE::_analyse_def_use(int line)
    { use1[line] = x->getName(); use2[line] = t->getName(); use3[line] = a;}
void _eFUNCRET::_analyse_def_use(int line)
    { def[line] = a->getName(); }
void _ePARAM::_analyse_def_use(int line)
    { use1[line] = t->getName(); }
void _eIFGOTO::_analyse_def_use(int line)
    { use1[line] = t1->getName(); use2[line] = t2->getName(); }
void _eGOTO::_analyse_def_use(int line) {}
void _eLABEL::_analyse_def_use(int line) {}
void _eRETVOID::_analyse_def_use(int line) {}
void _eRET::_analyse_def_use(int line)
    { use1[line] = t->getName(); }
void _eCALL::_analyse_def_use(int line) {}

typedef enum {
    DEFVAR, DEFARR, DIRECT, UNARY, BINARY, SEEK, SAVE,
    FUNCRET, CALL, PARAM, IFGOTO, GOTO, LABEL, RET, RETVOID,
    DEAD
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
bool reserved[maxlines];

vector<int> adj[maxlines];
vector<int> adj_rev[maxlines];
queue<int> que;

void _refresh(int line) {
    nxt[line] = false; jmp[line].clear(); type[line] = DEAD;
    use1[line].clear(); use2[line].clear(); use3[line].clear(); def[line].clear();
    if (seq[line]) 
        { seq[line]->_analyse_cf(line); seq[line]->_analyse_def_use(line); }
}
void _control_graph() {
    for (int i = 0; i < n; i++) adj[i].clear();
    for (int i = 0; i < n; i++) adj_rev[i].clear();
    for (int i = 0; i < n; i++) {
        if (nxt[i] && i + 1 < n) adj[i].push_back(i + 1);
        if (!jmp[i].empty()) adj[i].push_back(label[jmp[i]]);
    }
    for (int i = 0; i < n; i++)
    for (auto j: adj[i]) adj_rev[j].push_back(i);
}
void _analyse_liveness(string var_name) {
    /* var ought to be local */
    assert(!regManager->isglobal(var_name));
    bitset<maxlines> &active = regManager->vars[var_name]->active;
    active.reset(); assert(que.empty());
    for (int i = 0; i < n; i++)
        if (use1[i] == var_name || use2[i] == var_name || use3[i] == var_name)
            que.push(i), active[i] = true;
    while (!que.empty()) {
        int i = que.front(); que.pop();
        for (auto j: adj_rev[i]) {
            /* backward analysis */
            if (def[j] != var_name) {
                if (!active[j])
                    que.push(j), active[j] = true;
            }
            else reserved[j] = true;
        }
    }
}
bool cmp(string var1, string var2) {
    return regManager->vars[var1]->active.count() < regManager->vars[var2]->active.count();
}

bool _is_const(string var_name) {
    if (var_name.empty()) return true;
    if (var_name[0] == '-' || var_name[0] == '+') return true;
    if (var_name[0] >= '0' && var_name[0] <= '9') return true;
    return false;
}
bool _is_global(string var_name) {
    if (_is_const(var_name)) return false;
    return regManager->isglobal(var_name);
}
bool _is_param(string var_name) {
    return !_is_const(var_name) && var_name[0] == 'p';
}

bool reachable[maxlines];
bool _is_only_source(string var_name, int i, int j) {
    if (_is_const(var_name)) return true;
        
    memset(reachable, false, n); assert(que.empty());
    if (_is_param(var_name)) {
        /* param */
        if (!reachable[0])
            que.push(0), reachable[0] = true;
    }
    for (int u = 0; u < n; u++)
    if (def[u] == var_name && u != j) {
        for (auto v: adj[u])
            if (!reachable[v]) que.push(v), reachable[v] = true;
    }
    while (!que.empty()) {
        int u = que.front(); que.pop();
        if (u == j) continue;
        if (def[u] == var_name) continue;
        for (auto v: adj[u])
        if (!reachable[v]) /* forward analysis */
            que.push(v), reachable[v] = true;
    }
    return !reachable[i];
}
bool _is_available(int i, int j) {
    /* return if def[j] is available in line j */    
    memset(reachable, false, n); assert(que.empty());
    for (auto v: adj[j])
        if (!reachable[v]) que.push(v), reachable[v] = true;
    while (!que.empty()) {
        int u = que.front(); que.pop();
        if (def[u] == def[j]) continue;
        for (auto v: adj[u])
        if (!reachable[v]) /* forward analysis */
            que.push(v), reachable[v] = true;
    }
    return reachable[i];
}
bool _is_common_expr(int i, int j) {
    if (i == j) return false; /* same line */
    if (seq[j] == NULL) return false;
    if (type[i] != type[j]) return false;
    if (use1[i] != use1[j]) return false;
    if (use2[i] != use2[j]) return false;
    if (use3[i] != use3[j]) return false;
    if (def[j] == use1[j]) return false;
    if (def[j] == use2[j]) return false;
    if (def[j] == use3[j]) return false;
        /* guarantee def is different from every use in line j */
    if (_is_global(def[j])) return false;
    if (_is_global(use1[i])) return false;
    if (_is_global(use2[i])) return false;
    if (_is_global(use3[i])) return false;

    bool consistent = false;
    if (type[i] == UNARY) {
        _eUNARY *ei = (_eUNARY *)seq[i], *ej = (_eUNARY *)seq[j];
        consistent = (ei->op == ej->op);
    }
    if (type[i] == BINARY) {
        _eBINARY *ei = (_eBINARY *)seq[i], *ej = (_eBINARY *)seq[j];
        consistent = (ei->op == ej->op);
    }
    if (!consistent) return false;
    if (!_is_available(i, j)) return false;
    if (!_is_only_source(def[j], i, j)) return false;
    if (!_is_only_source(use1[j], i, j)) return false;
    if (!_is_only_source(use2[j], i, j)) return false;
    if (!_is_only_source(use3[j], i, j)) return false;

    return true;
}
void _common_expr_reduction(int i, int j) {
    /* line j: a = b + c */
    /* line i: x = b + c */
    /*-> line i: x = a */
    assert(i != j);
    if (type[i] == UNARY) {
        _eUNARY *ei = (_eUNARY *)seq[i], *ej = (_eUNARY *)seq[j];
        if (ei->op == ej->op) {
            seq[i] = new _eDIRECT(ei->a, ej->a);
            _refresh(i); return ;
        }
    }
    if (type[i] == BINARY) {
        _eBINARY *ei = (_eBINARY *)seq[i], *ej = (_eBINARY *)seq[j];
        if (ei->op == ej->op) {
            seq[i] = new _eDIRECT(ei->a, ej->a);
            _refresh(i); return ;
        }
    } 
}
void _analyse_common_expr(int i) {
    if (seq[i] == NULL) return ;
    for (int j = 0; j < n; j++)
    if (_is_common_expr(i, j)) {
        cerr << "line." << i << " finds common expr in line." << j << endl;
        _common_expr_reduction(i, j); return ;
    }
}
void _analyse_pass_self(int i) {
    if (type[i] == DIRECT) {
        _eDIRECT *ei = (_eDIRECT *)seq[i];
        if (ei->a->getName() == ei->t->getName())
            {seq[i] = NULL; _refresh(i); return ;}
    }
}

void _eFUNC::optimize() {
    cerr << "anaylize function " << func << endl;
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
    /* ======================== */
    /*  def-use analysis        */
    /* ======================== */
    /* unnecessary to clear map-table label */
    for (int i = 0; i < n; i++) _refresh(i);
    _control_graph();

#define _analysis_level 1
    int _analysis_iter = 0;
analysis:
    /* ======================== */
    /*  common expr analysis    */
    /* ======================== */
    for (int i = 0; i < n; i++)
        if (seq[i]) _analyse_common_expr(i);
    /* ======================== */
    /*  pass-self analysis      */
    /* ======================== */
    //for (int i = 0; i < n; i++)
    //    if (seq[i]) _analyse_pass_self(i);
    
    /* ======================== */
    /*  liveness analysis       */
    /* ======================== */
    memset(reserved, false, n);
    for (auto var_name: var_list)
        _analyse_liveness(var_name);
    for (int i = 0; i < arity; i++)
        _analyse_liveness("p" + to_string(i));
    for (int i = 0; i < n; i++)
    if (!reserved[i]) {
        if (seq[i] == NULL) continue;
        if (def[i].empty()) continue;
        Variable *var = regManager->vars[def[i]];
        if (var->isglobal()) continue;
        /* global vars def-use condition is much more complicated */
        if (type[i] == FUNCRET) {
            seq[i] = new _eCALL(((_eFUNCRET *) seq[i])->func);
            _refresh(i); continue;
            /* though the returned value is not used,
                function call could have side effects */
        }
        seq[i] = NULL; _refresh(i);
    }
    
    if (++_analysis_iter < _analysis_level) goto analysis;

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
        if (seq[currentLine] && !seq[currentLine]->isdef())
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
        if (decl->isdef() && regManager->isvar(decl->getName())) {
            regManager->try_allocate(decl->getName());
            global_var_list.push_back(decl->getName());
        }
    for (auto func: seq)
        if (!func->isdef()) func->translate();
    tiggerRoot = new _tSEQ(tiggerList);
}