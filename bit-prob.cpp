#include <bits/stdc++.h>
#ifndef defs_hpp
#include "defs.hpp"
#define defs_hpp
#endif

using namespace std;

extern int n;
extern vector<eeyoreAST *> alter;
extern vector<eeyoreAST *> seq;
extern typeAST type[maxlines];

extern bool _has_no_side_effect(string this_func) ;
extern bool _is_param_stable() ;
extern bool _is_call_once(string this_func) ;
extern bool _is_sensitive(string var_name) ;
extern bool _is_param(string var_name) ;
extern bool _not_use_var(string var_name, int line) ;


string lx, ly, lz, pi;
string lA, ret, lEnd;
string tj, tk, Tx;
eeyoreAST *R0, *R1, *R2, *R3;

vector<eeyoreAST *> stmts3_4, stmts5, stmts6;
string cpower[32];

void _best_multiply() {
    alter.clear();
    string ret = varManager->newT(false);
    string b0 = varManager->newT(false);
    string b1 = varManager->newT(false);
    alter.push_back(new _eDEFVAR(ret, false));
    alter.push_back(new _eDEFVAR(b0, false));
    alter.push_back(new _eDEFVAR(b1, false));
    string loop = varManager->newl();
    string exit = varManager->newl();
    string evenx1 = varManager->newl();
    string eveny1 = varManager->newl();
    string even0 = varManager->newl();
    
    
    alter.push_back(new _eDIRECT(new _eVAR(ret), new _eNUM(0)));
    alter.push_back(new _eLABEL(loop));
    alter.push_back(new _eIFGOTO(new _eVAR("p1"), "==", new _eNUM(0), exit));
    alter.push_back(new _eBINARY(new _eVAR(b0), new _eVAR("p1"), "&", new _eNUM(1)));
    alter.push_back(new _eBINARY(new _eVAR(b1), new _eVAR("p1"), "&", new _eNUM(2)));
    alter.push_back(new _eBINARY(new _eVAR("p0"), new _eVAR("p0"), "%u", new _eVAR("T0")));
    alter.push_back(new _eBINARY(new _eVAR("p1"), new _eVAR("p1"), "/", new _eNUM(4)));
    alter.push_back(new _eIFGOTO(new _eVAR(b0), "==", new _eNUM(0), even0));
    alter.push_back(new _eBINARY(new _eVAR(ret), new _eVAR(ret), "+", new _eVAR("p0")));
    alter.push_back(new _eBINARY(new _eVAR("p0"), new _eVAR("p0"), "*", new _eNUM(2)));
    alter.push_back(new _eIFGOTO(new _eVAR(b1), "==", new _eNUM(0), evenx1));
    alter.push_back(new _eBINARY(new _eVAR(ret), new _eVAR(ret), "+", new _eVAR("p0")));
    alter.push_back(new _eLABEL(evenx1));
    alter.push_back(new _eBINARY(new _eVAR("p0"), new _eVAR("p0"), "*", new _eNUM(2)));
    alter.push_back(new _eBINARY(new _eVAR(ret), new _eVAR(ret), "%u", new _eVAR("T0")));
    alter.push_back(new _eGOTO(loop));

    alter.push_back(new _eLABEL(even0));
    alter.push_back(new _eBINARY(new _eVAR("p0"), new _eVAR("p0"), "*", new _eNUM(2)));
    alter.push_back(new _eIFGOTO(new _eVAR(b1), "==", new _eNUM(0), eveny1));
    alter.push_back(new _eBINARY(new _eVAR(ret), new _eVAR(ret), "+", new _eVAR("p0")));
    alter.push_back(new _eBINARY(new _eVAR(ret), new _eVAR(ret), "%u", new _eVAR("T0")));
    alter.push_back(new _eLABEL(eveny1));
    alter.push_back(new _eBINARY(new _eVAR("p0"), new _eVAR("p0"), "*", new _eNUM(2)));
    alter.push_back(new _eGOTO(loop));

    alter.push_back(new _eLABEL(exit));
    alter.push_back(new _eRET(new _eVAR(ret)));
}
void _divide_conquer(int l, int r) {
    if (l == r) {
        alter.push_back(new _eGOTO(cpower[l - 1]));
        return ;
    }
    int m = l + r >> 1;
    string lbranch = (l < m) ? varManager->newl() : (l ? cpower[l - 1] : lEnd);
    alter.push_back(new _eIFGOTO(new _eVAR(pi), "<", new _eNUM(1 << m + 1), lbranch));
    
    _divide_conquer(m + 1, r);
    
    if (l < m) {
        alter.push_back(new _eLABEL(lbranch));
        _divide_conquer(l, m);
    }
}
void _calc_body(int index) {
    string dpower = varManager->newl();
    /* -> cpower: */
    alter.push_back(new _eLABEL(cpower[index]));
    /* -> stms3.. stmts4.. */
    alter.insert(alter.end(), stmts3_4.begin(), stmts3_4.end());
    /* -> tk = pi & $2^index$ */
    alter.push_back(new _eBINARY(new _eVAR(tk), new _eVAR(pi), "&", new _eNUM(1 << index)));
    /* -> if tk == 0 goto dpower */
    alter.push_back(new _eIFGOTO(new _eVAR(tk), "==", new _eNUM(0), dpower));
    /* -> stms5.. */
    alter.insert(alter.end(), stmts5.begin(), stmts5.end());
    /* -> goto $cpower-1$ */
    alter.push_back(new _eGOTO(index > 0 ? cpower[index - 1] : lEnd));
    /* -> dpower: */
    alter.push_back(new _eLABEL(dpower));
    /* -> stms6.. */
    alter.insert(alter.end(), stmts6.begin(), stmts6.end());

}
bool _pattern_matching_bit_prob(string this_func, int arity, bool def1) {
    if (!_has_no_side_effect(this_func)) return false;
    if (!_is_param_stable()) return false;
    if (!_is_call_once(this_func)) return false;
    
    /* assertion: every seq[line] ptr is not null */
    for (int i = 0; i < n; i++)
        assert(seq[i] != NULL);

    alter.clear(); stmts3_4.clear(); stmts5.clear(); stmts6.clear();

    lA = varManager->newl();
    lEnd = varManager->newl();
    ret = varManager->newT(false);
    alter.push_back(new _eDEFVAR(ret, false));

    int line = 0;

    /* decls */
    for (; line < n; line++) {
        if (type[line] != DEFARR && type[line] != DEFVAR) break;
        alter.push_back(seq[line]);
    }
    
    /* if pi != 0 goto lx */
    if (line >= n) return false;
    if (type[line] != IFGOTO) return false;
    else {
        _eIFGOTO *stmt = (_eIFGOTO *) seq[line];
        pi = stmt->t1->getName();
        if (!_is_param(pi)) return false;
        if (stmt->op != "!=") return false;
        if (stmt->t2->getName() != "0") return false;
        lx = stmt->l; ++line;
    }
    
    /* -> if pi != 0 goto lA */
    alter.push_back(new _eIFGOTO(new _eVAR(pi), "!=", new _eNUM(0), lA));

    /* stmts1, don't use pi */
    for (; line < n && type[line] != RET; line++) {
        if (!_not_use_var(pi, line)) return false;
        alter.push_back(seq[line]);
    }

    /* return R0 */
    if (line >= n) return false;
    if (type[line] != RET) return false;
    else {
        if (!_not_use_var(pi, line)) return false;
        _eRET *stmt = (_eRET *) seq[line];
        R0 = stmt->t;
        alter.push_back(seq[line]); ++line;
    }

    /* lx: */
    if (line >= n) return false;
    if (type[line] != LABEL) return false;
    else {
        _eLABEL *stmt = (_eLABEL *) seq[line];
        if (stmt->l != lx) return false;
        ++line;
    }

    /* -> lA: */
    alter.push_back(new _eLABEL(lA));

    /* if pi != 1 goto ly */
    if (line >= n) return false;
    if (type[line] != IFGOTO) return false;
    else {
        _eIFGOTO *stmt = (_eIFGOTO *) seq[line];
        if (stmt->t1->getName() != pi) return false;
        if (stmt->op != "!=") return false;
        if (stmt->t2->getName() != "1") return false;
        ly = stmt->l; ++line;
    }

    /* stmts2, don't use pi */
    for (; line < n && type[line] != RET; line++) {
        if (!_not_use_var(pi, line)) return false;
        alter.push_back(seq[line]);
    }

    /* return R1 */
    if (line >= n) return false;
    if (type[line] != RET) return false;
    else {
        if (!_not_use_var(pi, line)) return false;
        _eRET *stmt = (_eRET *) seq[line];
        R1 = stmt->t; ++line;
    }
    
    /* ly: */
    if (line >= n) return false;
    if (type[line] != LABEL) return false;
    else {
        _eLABEL *stmt = (_eLABEL *) seq[line];
        if (stmt->l != ly) return false;
        ++line;
    }

    /* -> ret = R1 */
    alter.push_back(new _eDIRECT(new _eVAR(ret), R1));
    
    if (arity == 0) return false;

    /* stmts3, don't use pi */
    for (; line < n && type[line] != PARAM; line++) {
        if (_not_use_var(pi, line))
            stmts3_4.push_back(seq[line]);
        else {
            if (type[line] != BINARY) return false;
            _eBINARY *stmt = (_eBINARY *) seq[line];
            tj = stmt->a->getName();
            if (stmt->t1->getName() != pi) return false;
            if (stmt->op != "/") return false;
            if (stmt->t2->getName() != "2") return false;
        }
    }

    /* param .. */
    for (int i = 0; i < arity; i++, ++line) {
        if (line >= n) continue;
        string param = "p" + to_string(i);
        if (type[line] != PARAM) return false;
        _ePARAM *stmt = (_ePARAM *)seq[line];
        if (param != pi) {
            if (stmt->t->getName() != param) return false;
        }
        else {
            if (stmt->t->getName() != tj) return false;
        }
    }
    
    /* Tx = call f_this_func */
    if (line >= n) return false;
    if (type[line] != FUNCRET) return false;
    else {
        if (!_not_use_var(pi, line)) return false;
        _eFUNCRET *stmt = (_eFUNCRET *) seq[line];
        if (this_func != stmt->func) return false;
        Tx = stmt->a->getName(); ++line;
    }

    /* -> Tx = ret */
    stmts3_4.push_back(new _eDIRECT(new _eVAR(Tx), new _eVAR(ret)));
    
    /* stmts4, don't use pi */
    for (; line < n && type[line] != IFGOTO; line++) {
        if (_not_use_var(pi, line))
            stmts3_4.push_back(seq[line]);
        else {
            if (type[line] != BINARY) return false;
            _eBINARY *stmt = (_eBINARY *) seq[line];
            tk = stmt->a->getName();
            if (stmt->t1->getName() != pi) return false;
            if (stmt->op != "%") return false;
            if (stmt->t2->getName() != "2") return false;
        }
    }
    /* if tk != 1 goto lz */
    if (line >= n) return false;
    if (type[line] != IFGOTO) return false;
    else {
        _eIFGOTO *stmt = (_eIFGOTO *) seq[line];
        if (stmt->t1->getName() != tk) return false;
        if (stmt->op != "!=") return false;
        if (stmt->t2->getName() != "1") return false;
        lz = stmt->l; ++line;
    }

    /* stmts5, don't use pi */
    for (; line < n && type[line] != RET; line++) {
        if (!_not_use_var(pi, line)) return false;
        stmts5.push_back(seq[line]);
    }
    /* return R2 */
    if (line >= n) return false;
    if (type[line] != RET) return false;
    else {
        if (!_not_use_var(pi, line)) return false;
        _eRET *stmt = (_eRET *) seq[line];
        R2 = stmt->t; ++line;
    }

    /* -> ret = R2 */
    stmts5.push_back(new _eDIRECT(new _eVAR(ret), R2));
    
    while (line < n && type[line] != LABEL) ++line;
    /* there might be some useless statements after return */

    /* lz: */
    if (line >= n) return false;
    if (type[line] != LABEL) return false;
    else {
        _eLABEL *stmt = (_eLABEL *) seq[line];
        if (stmt->l != lz) return false;
        ++line;
    }

    /* stmts6, don't use pi */
    for (; line < n && type[line] != RET; line++) {
        if (!_not_use_var(pi, line)) return false;
        stmts6.push_back(seq[line]);
    }
    /* return R3 */
    if (line >= n) return false;
    if (type[line] != RET) return false;
    else {
        if (!_not_use_var(pi, line)) return false;
        _eRET *stmt = (_eRET *) seq[line];
        R3 = stmt->t; ++line;
    }
    /* -> ret = R3 */
    stmts6.push_back(new _eDIRECT(new _eVAR(ret), R3));
    
    for (int i = 30; i >= 0; i--) cpower[i] = varManager->newl();
    _divide_conquer(0, 30);
    for (int i = 30; i >= 0; i--) _calc_body(i);

    /* -> lEnd: */
    alter.push_back(new _eLABEL(lEnd));
    /* -> return ret */
    alter.push_back(new _eRET(new _eVAR(ret)));

    cerr << this_func << " is re-written as a bit-prob function" << endl;
    // for (auto stmt: alter) stmt->Dump(); fflush(stdout);
    if (def1) _best_multiply();
    seq = alter; n = seq.size(); return true;
}