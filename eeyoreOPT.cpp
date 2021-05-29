#include <bits/stdc++.h>
#ifndef defs_hpp
#include "defs.hpp"
#define defs_hpp
#endif
using namespace std;

int currentLine;

string def[maxlines], use1[maxlines], use2[maxlines], use3[maxlines];
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
vector<eeyoreAST *> alter;
vector<string> var_list; 
vector<string> global_var_list {};
int n;
bool reserved[maxlines];

vector<int> adj[maxlines];
vector<int> adj_rev[maxlines];
queue<int> que;

void _refresh(int line) {
    nxt[line] = true; jmp[line].clear(); type[line] = DEAD;
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
    return regManager->vars[var1]->active.count()
        > regManager->vars[var2]->active.count();
}

bool _is_const(string var_name) {
    if (var_name.empty()) return true;
    if (var_name[0] == '-' || var_name[0] == '+') return true;
    if (var_name[0] >= '0' && var_name[0] <= '9') return true;
    return false;
}
bool _is_sensitive(string var_name) {
    if (_is_const(var_name)) return false;
    return regManager->isglobal(var_name) && regManager->isvar(var_name) && !regManager->isconst(var_name);
}
bool _is_param(string var_name) {
    return !_is_const(var_name) && var_name[0] == 'p';
}

bool reachable[maxlines];
bool _is_only_source(string var_name, int x, int o) {
    if (_is_const(var_name)) return true;
    if (_is_sensitive(var_name)) return false;
        
    memset(reachable, false, n); assert(que.empty());
    if (_is_param(var_name)) {
        /* param */
        if (!reachable[0])
            que.push(0), reachable[0] = true;
    }
    for (int u = 0; u < n; u++)
    if (def[u] == var_name && u != o) {
        for (auto v: adj[u])
            if (!reachable[v]) que.push(v), reachable[v] = true;
    }
    while (!que.empty()) {
        int u = que.front(); que.pop();
        if (u == o) continue;
        if (def[u] == var_name) continue;
        for (auto v: adj[u])
        if (!reachable[v]) /* forward analysis */
            que.push(v), reachable[v] = true;
    }
    return !reachable[x];
}
bool _is_available(int x, int o) {
    /* line[j] must occurs before line[i] in abs path */    
    memset(reachable, false, n); assert(que.empty());
    reachable[x] = true; que.push(x);
    while (!que.empty()) {
        int u = que.front(); que.pop();
        if (u == o) continue;
        for (auto v: adj_rev[u])
        if (!reachable[v]) /* backward analysis */
            que.push(v), reachable[v] = true;
    }
    return !reachable[0];
}
bool _is_clean(string arr_name, int x, int o) {
    /* check if this array is contaminated during the data-path */
    assert(!arr_name.empty());
    memset(reachable, false, n); assert(que.empty());
    /* for global vars, any function-call is unsave,
        for local vars, param arr_name is unsave*/
    reachable[o] = true; que.push(o);
    while (!que.empty()) {
        int u = que.front(); que.pop();
        for (auto v: adj[u])
        if (!reachable[v]) /* forward analysis */
            que.push(v), reachable[v] = true;
    }
    assert(reachable[x]);
    /* x must be accessible by o */
    for (int i = 0; i < n; i++)
    if (reachable[i]) {
        bool dirty = false;
        switch (type[i]) {
            case SAVE: dirty = true; break;
            case CALL: dirty = true; break;
            case FUNCRET: dirty = true; break;
            default: dirty = false;
        }
        if (dirty) que.push(i);
        else reachable[i] = false;
    }
    while (!que.empty()) {
        int u = que.front(); que.pop();
        for (auto v: adj[u])
        if (!reachable[v]) /* forward analysis */
            que.push(v), reachable[v] = true;
    }
    return !reachable[x];
}
bool _is_common_expr(int x, int o) {
    if (x == o) return false; /* same line */
    if (seq[o] == NULL) return false;
    if (type[x] != type[o]) return false;
    if (use1[x] != use1[o]) return false;
    if (use2[x] != use2[o]) return false;
    if (use3[x] != use3[o]) return false;
    if (def[o] == use1[o]) return false;
    if (def[o] == use2[o]) return false;
    if (def[o] == use3[o]) return false;
        /* guarantee def is different from every use in line j */

    bool consistent = false;
    if (type[o] == UNARY) {
        _eUNARY *ex = (_eUNARY *)seq[x], *eo = (_eUNARY *)seq[o];
        consistent = (ex->op == eo->op);
    }
    if (type[o] == BINARY) {
        _eBINARY *ex = (_eBINARY *)seq[x], *eo = (_eBINARY *)seq[o];
        consistent = (ex->op == eo->op);
    }
    if (type[o] == SEEK) {
        consistent = true;
    }
    if (!consistent) return false;
    if (!_is_available(x, o)) return false;
    if (!_is_only_source(def[o], x, o)) return false;
    if (!_is_only_source(use1[o], x, o)) return false;
    if (!_is_only_source(use2[o], x, o)) return false;
    if (!_is_only_source(use3[o], x, o)) return false;
    if (type[o] == SEEK && !_is_clean(use3[o], x, o)) return false;
    
    if (type[o] == SEEK) cerr << "SEEK!";
    return true;
}
void _common_expr_reduction(int x, int o) {
    /* line o: a = b + c */
    /* line x: x = b + c */
    /*-> line x: x = a */
    assert(x != o);
    if (type[o] == UNARY) {
        _eUNARY *ex = (_eUNARY *)seq[x], *eo = (_eUNARY *)seq[o];
        if (ex->op == eo->op) {
            seq[x] = new _eDIRECT(ex->a, eo->a);
            _refresh(x); return ;
        }
    }
    if (type[o] == BINARY) {
        _eBINARY *ex = (_eBINARY *)seq[x], *eo = (_eBINARY *)seq[o];
        if (ex->op == eo->op) {
            seq[x] = new _eDIRECT(ex->a, eo->a);
            _refresh(x); return ;
        }
    } 
    if (type[o] == SEEK) {
        _eSEEK *ex = (_eSEEK *)seq[x], *eo = (_eSEEK *)seq[o];
        seq[x] = new _eDIRECT(ex->a, eo->a);
        _refresh(x); return ;
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
        _eDIRECT *ex = (_eDIRECT *)seq[i];
        if (ex->a->getName() == ex->t->getName())
            { cerr << "eliminate self-pass in line." << i << ": " << ex->a->getName() << endl; seq[i] = NULL; _refresh(i); return ;}
    }
}
void _analyse_const_pass(int i) {
    if (type[i] == UNARY) {
        _eUNARY *ex = (_eUNARY *)seq[i];
        if (ex->t->isnum()) {
            cerr << "const-pass in line." << i << ": " << ex->a->getName() << endl;
            seq[i] = new _eDIRECT(ex->a, new _eNUM(unary_result(ex->op, ex->t->getInt())));
            _refresh(i); return ;
        }
    }
    if (type[i] == BINARY) {
        _eBINARY *ex = (_eBINARY *)seq[i];
        if (ex->t1->isnum() && ex->t2->isnum()) {
            cerr << "const-pass in line." << i << ": " << ex->a->getName() << endl;
            seq[i] = new _eDIRECT(ex->a, new _eNUM(binary_result(ex->t1->getInt(), ex->op, ex->t2->getInt())));
            _refresh(i); return ;
        }
    }
    if (type[i] == IFGOTO) {
        _eIFGOTO *ex = (_eIFGOTO *)seq[i];
        if (ex->t1->isnum() && ex->t2->isnum()) {
            cerr << "const-pass in line." << i << ": goto" << ex->l << endl;
            if (binary_result(ex->t1->getInt(), ex->op, ex->t2->getInt()))
                seq[i] = new _eGOTO(ex->l);
            else
                seq[i] = NULL;
            _refresh(i); return ;
        }
    }
}
int _only_def(string var_name, int x) {
    int def_pos = -1, def_cnt = 0;
    if (_is_const(var_name)) return -1;
    if (_is_sensitive(var_name)) return -1;
    memset(reachable, false, n); assert(que.empty());
    for (auto v: adj_rev[x])
        if (!reachable[v]) que.push(v), reachable[v] = true;
    while (!que.empty()) {
        int u = que.front(); que.pop();
            /* backward analysis */
        if (def[u] != var_name) {
            for (auto v: adj_rev[u])
            if (!reachable[v]) /* back_ward analysis */
                que.push(v), reachable[v] = true;
        }
        else
            { def_pos = u; ++def_cnt; }
    }
    return def_cnt != 1 ? -1: def_pos;
}

void _analyse_direct(eeyoreAST *&x, int line) {
    string var_name = x->getName();
    int def_pos = _only_def(var_name, line);
    if (def_pos == -1) return ;
    if (type[def_pos] != DIRECT) return ;
    assert(def_pos != line);
    _eDIRECT *ed = (_eDIRECT *)seq[def_pos];
    assert(ed->a->getName() == var_name);
    if (!_is_only_source(ed->t->getName(), line, def_pos)) return ;
    if (_is_const(ed->t->getName()) && !isint12(ed->t->getInt())) return ;
    x = ed->t; _refresh(line);
    cerr << "line." << line << " alter " << var_name << " to " << ed->t->getName() << endl;
}
void _eDEFVAR::_analyse_direct_pass(int line) {}
void _eDEFARR::_analyse_direct_pass(int line) {}
void _eDIRECT::_analyse_direct_pass(int line)
    { _analyse_direct(t, line); }
void _eUNARY::_analyse_direct_pass(int line)
    { _analyse_direct(t, line); }
void _eBINARY::_analyse_direct_pass(int line)
    { _analyse_direct(t1, line); _analyse_direct(t2, line); }
void _eSEEK::_analyse_direct_pass(int line)
    { _analyse_direct(x, line); }
void _eSAVE::_analyse_direct_pass(int line)
    { _analyse_direct(x, line); _analyse_direct(t, line);}
void _eFUNCRET::_analyse_direct_pass(int line) {}
void _ePARAM::_analyse_direct_pass(int line)
    { _analyse_direct(t, line); }
void _eIFGOTO::_analyse_direct_pass(int line)
    { _analyse_direct(t1, line); _analyse_direct(t2, line); }
void _eGOTO::_analyse_direct_pass(int line) {}
void _eLABEL::_analyse_direct_pass(int line) {}
void _eRETVOID::_analyse_direct_pass(int line) {}
void _eRET::_analyse_direct_pass(int line)
    { _analyse_direct(t, line); }
void _eCALL::_analyse_direct_pass(int line) {}

map<string, bool> _no_side_effect;
bool _is_call_self(string this_func, int line) {
    return type[line] == FUNCRET && this_func == ((_eFUNCRET *)seq[line]) -> func;
}
bool _has_no_side_effect_wrapin(string this_func) {
    for (int i = 0; i < n; i++) {
        if (type[i] == SAVE) return false;
        if (type[i] == SEEK) return false;
        if (type[i] == CALL) return false;
        if (type[i] == FUNCRET && !_is_call_self(this_func, i)) {
            if (!_no_side_effect[((_eFUNCRET *)seq[i]) -> func]) return false;
        }
        /* potential optimization here: called function might be no side-effect too */
        if (_is_sensitive(def[i])) return false;
        if (_is_sensitive(use1[i])) return false;
        if (_is_sensitive(use2[i])) return false;
        if (_is_sensitive(use3[i])) return false;
    }
    return true;
}
bool _has_no_side_effect(string this_func) {
    return _no_side_effect[this_func] = _has_no_side_effect_wrapin(this_func);
}
bool _is_param_stable() {
    for (int i = 0; i < n; i++)
        if (_is_param(def[i])) return false;
    return true;
}
bool _is_call_once(string this_func) {
    int call_cnt = 0;
    for (int i = 0; i < n; i++) {
        if (_is_call_self(this_func, i)) call_cnt += 1;
        if (type[i] == FUNCRET && call_cnt == 0) return false;
    }
        /* for convenience, call-self is assumed as the first function call. */
    return call_cnt == 1;
}

extern bool _pattern_matching_bit_prob(string this_func, int arity, bool def1);

bool _not_use_var(string var_name, int line) {
    if (def[line] == var_name) return false;
    if (use1[line] == var_name) return false;
    if (use2[line] == var_name) return false;
    if (use3[line] == var_name) return false;
    return true;
}

bool _is_static_in_interval(string var_name, int label, int loop) {
    if (_is_const(var_name)) return true;
    if (_is_sensitive(var_name)) return false;
    for (int i = label; i <= loop; i++)
        if (def[i] == var_name) return false;
    return true;
}
bool _is_modified_once_in_interval(string var_name, int label, int loop, int modified_line) {
    if (_is_const(var_name)) return true;
    if (_is_sensitive(var_name)) return false;
    for (int i = label; i <= loop; i++)
        if (i != modified_line && def[i] == var_name) return false;
    return true;
}
bool _is_pure_assign(int line) {
    return seq[line] != NULL && (type[line] == BINARY || type[line] == UNARY || type[line] == DIRECT);
}
vector<eeyoreAST*> liftup;
void _analyse_lift(int label) {
    string l = ((_eLABEL *)seq[label]) -> l;
    for (int i = 0; i < label; i++) {
        if (type[i] == GOTO && ((_eGOTO *)seq[i])->l == l) return ;
        if (type[i] == IFGOTO && ((_eIFGOTO *)seq[i])->l == l) return ;
    }
    int loop = -1, loop_cnt = 0;
    for (int i = label + 1; i < n; i++) {
        if (type[i] == GOTO && ((_eGOTO *)seq[i])->l == l)
            { loop = i; loop_cnt += 1; }
        if (type[i] == IFGOTO && ((_eIFGOTO *)seq[i])->l == l) return ;
    }
    if (loop_cnt != 1) return ;

    liftup.clear();
    for (int i = label; i <= loop; i++)
    if (_is_pure_assign(i)) {
        if (!_is_static_in_interval(use1[i], label, loop)) continue;
        if (!_is_static_in_interval(use2[i], label, loop)) continue;
        if (!_is_static_in_interval(use3[i], label, loop)) continue;
        if (!_is_modified_once_in_interval(def[i], label, loop, i)) continue;
        cerr << "lift line." << i << endl;
        liftup.push_back(seq[i]); seq[i] = NULL; _refresh(i);
    }
    for (int i = label; i <= loop; i++)
        if (seq[i] != NULL) {liftup.push_back(seq[i]); seq[i] = NULL;}
    for (int i = label, j = 0; j < liftup.size() ; i++, j++)
        seq[i] = liftup[j];

    cerr << "loop detected between line." << label << " and line." << loop << endl;
    

    for (int i = 0; i < n; i++) _refresh(i);
    _control_graph();
}
void _eFUNC::optimize() {
    cerr << "anaylizing function " << func << endl;
    seq = ((_eSEQ *) body)->seq;
    n = seq.size();
    assert(n < maxlines);
    _tFUNC *tfunc = new _tFUNC(func, arity);
    regManager->new_environ();

    /* ======================== */
    /* pattern matching         */
    /* ======================== */
    for (int i = 0; i < n; i++) _refresh(i);
    _control_graph();
    // if (!_pattern_matching_bit_prob(func, arity, false))
    _pattern_matching_bit_prob(func, arity, true);

    /* ================ */
    /*  param decl      */
    /* ================ */
    var_list.clear();
    for (int i = 0; i < arity; i++)
        regManager->newLocal("p" + to_string(i), 4, true, false);
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

#define _analysis_level 10
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
    for (int i = 0; i < n; i++)
        if (seq[i]) _analyse_pass_self(i);
    /* ======================== */
    /*  direct-pass analysis    */
    /* ======================== */
    for (int i = 0; i < n; i++)
        if (seq[i]) seq[i]->_analyse_direct_pass(i);
    /* ======================== */
    /*  const analysis          */
    /* ======================== */
    for (int i = 0; i < n; i++)
        if (seq[i]) _analyse_const_pass(i);
    /* ======================== */
    /*  stmt lift               */
    /* ======================== */
    for (int i = 0; i < n; i++)
        if (type[i] == LABEL) _analyse_lift(i);
    
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
        tiggerStmt(new _tDIRECT(reserved_x1, 1)); /* x1 always stores 1*/
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
        if (decl->isdef()) {
            regManager->try_allocate(decl->getName());
            global_var_list.push_back(decl->getName());
        }
    for (auto func: seq)
        if (!func->isdef()) func->translate();
    tiggerRoot = new _tSEQ(tiggerList);
}