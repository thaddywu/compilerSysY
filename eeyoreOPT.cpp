#ifndef defs_hpp
#include "defs.hpp"
#define defs_hpp
#endif
using namespace std;

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

#define maxlines 1000
typedef enum {
    DEFVAR, DEFARR, DIRECT, UNARY, BINARY, SEEK, SAVE,
    FUNCRET, CALL, PARAM, IFGOTO, GOTO, LABEL, RET, RETVOID
}typeAST;
typedef enum {
    VAR, ARR, NUM
}typeATOM;
typeAST type[maxlines];
map<string, int> label;
map<string, typeATOM> local;
bool nxt[maxlines]; string jmp[maxlines];
void _eDEFVAR::_analyse_cf(int line) { nxt[line] = true; type[line] = DEFARR; local[var] = VAR; }
void _eDEFARR::_analyse_cf(int line) { nxt[line] = true; type[line] = DEFARR; local[var] = ARR; }
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

vector<eeyoreAST *> seq, opt_seq;
string def[maxlines], use1[maxlines], use2[maxlines];
bitset<maxlines> reach[maxlines];
bool reserved[maxlines];
int n;

bool visited[maxlines];
bool _analyse_reach(const string &var, int pos, int start, bool warmup) {
    if (visited[pos]) return reach[start][pos];
    bool used = false; visited[pos] = true;
    if (!warmup) {
        if (use1[pos] == var) used = true;
        if (use2[pos] == var) used = true;
        if (def[pos] == var) return reach[start][pos] = used;
    }
    if (nxt[pos] && pos + 1 < n)
        used |= _analyse_reach(var, pos + 1, start, false);
    if (!jmp[pos].empty())
        used |= _analyse_reach(var, label[jmp[pos]], start, false);
    return reach[start][pos] = used;
}
void _eFUNC::optimize() {
    seq = ((_eSEQ *) body)->seq; n = seq.size();
    memset(reserved, true, n);
    cout << func << " " << arity << " " << n << endl;
    /* inner procedural control flow analysis */
    label.clear(); local.clear();
    for (int i = 0; i < n; i++) {
        jmp[i].clear(); nxt[i] = false;
        seq[i]->_analyse_cf(i);
    }

    for (int i = 0; i < n; i++) {
        def[i].clear(); use1[i].clear(); use2[i].clear();
        seq[i]->_analyse_def_use(def[i], use1[i], use2[i]); 
    }
    for (int i = 0; i < n; i++) {
        reach[i].reset();
        if (def[i].empty()) continue;
        if (local.find(def[i]) == local.end()) continue; /* skip global */
        memset(visited, false, n);
        if (!_analyse_reach(def[i], i, i, true))
            reserved[i] = false;
    }
    for (int i = 0; i < n; i++)
        { printf("%3d:\t", i); seq[i]->Dump(); printf("... [%s] def:%s use1:%s use2:%s [nxt%d] [jmp:%s]\n", reserved[i] ? "true":"false", def[i].c_str(), use1[i].c_str(), use2[i].c_str(), (int)nxt[i], jmp[i].c_str()); }

    opt_seq.clear();
    for (int i = 0; i < n; i++)
        if (reserved[i]) opt_seq.push_back(seq[i]);
    ((_eSEQ *) body)->seq = opt_seq;
}