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

bool _pattern_matching_bit_prob(string this_func, int arity, bool def1) {
    if (!_has_no_side_effect(this_func)) return false;
    if (!_is_param_stable()) return false;
    if (!_is_call_once(this_func)) return false;
    
    /* assertion: every seq[line] ptr is not null */
    for (int i = 0; i < n; i++)
        assert(seq[i] != NULL);

    alter.clear();

    string lx, ly, lz, pi;
    string lA = varManager->newl();
    string lB = varManager->newl();
    string lC = varManager->newl();
    string lD = varManager->newl();
    string lE = varManager->newl();
    string lF = varManager->newl();
    string lEnd = varManager->newl();
    string ret = varManager->newT(false);
    string bit = varManager->newT(false); /* problematic here */
    string tj, tk, Tx;
    eeyoreAST *R0, *R1, *R2, *R3;
    alter.push_back(new _eDEFVAR(ret, false));
    alter.push_back(new _eDEFVAR(bit, false));

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
    
    /* -> bit = 1 << 20 */
    alter.push_back(new _eDIRECT(new _eVAR(bit), new _eNUM(1 << 20)));
    /* -> if bit > pi goto lF */
    alter.push_back(new _eIFGOTO(new _eVAR(bit), ">", new _eVAR(pi), lF));
    /* -> bit = 1 << 30 */
    alter.push_back(new _eDIRECT(new _eVAR(bit), new _eNUM(1 << 30)));
    /* -> lF: */
    alter.push_back(new _eLABEL(lF));
    /* -> lB: */
    alter.push_back(new _eLABEL(lB));
    /* -> if bit <= pi goto lC */
    alter.push_back(new _eIFGOTO(new _eVAR(bit), "<=", new _eVAR(pi), lC));
    /* -> bit = bit / 2 */
    alter.push_back(new _eBINARY(new _eVAR(bit), new _eVAR(bit), "/", new _eNUM(2)));
    /* -> goto lB */
    alter.push_back(new _eGOTO(lB));
    /* -> lC: */
    alter.push_back(new _eLABEL(lC));

if (!def1) goto no_def1_1;

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


goto def1_1;
no_def1_1:

    /* -> bit = bit * 2 */
    alter.push_back(new _eBINARY(new _eVAR(bit), new _eVAR(bit), "*", new _eNUM(2)));
    /* -> ret = R0 */
    alter.push_back(new _eDIRECT(new _eVAR(ret), R0));

def1_1:

    /* -> lD: */
    alter.push_back(new _eLABEL(lD));
    /* -> if bit == 1 goto lEnd */
    alter.push_back(new _eIFGOTO(new _eVAR(bit), "==", new _eNUM(1), lEnd));
    /* -> bit = bit / 2 */
    alter.push_back(new _eBINARY(new _eVAR(bit), new _eVAR(bit), "/", new _eNUM(2)));
    
    if (arity == 0) return false;

    /* stmts3, don't use pi */
    for (; line < n && type[line] != PARAM; line++) {
        if (_not_use_var(pi, line))
            alter.push_back(seq[line]);
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
    alter.push_back(new _eDIRECT(new _eVAR(Tx), new _eVAR(ret)));
    
    /* stmts4, don't use pi */
    for (; line < n && type[line] != IFGOTO; line++) {
        if (_not_use_var(pi, line))
            alter.push_back(seq[line]);
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

    /* -> tk = pi & bit */
    /* this operator is not supported by eeyore & tigger */
    alter.push_back(new _eBINARY(new _eVAR(tk), new _eVAR(pi), "&", new _eVAR(bit)));
    /* -> if tk != bit goto lE */
    alter.push_back(new _eIFGOTO(new _eVAR(tk), "!=", new _eVAR(bit), lE));

    /* stmts5, don't use pi */
    for (; line < n && type[line] != RET; line++) {
        if (!_not_use_var(pi, line)) return false;
        alter.push_back(seq[line]);
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
    alter.push_back(new _eDIRECT(new _eVAR(ret), R2));
    /* -> goto lD */
    alter.push_back(new _eGOTO(lD));
    /* -> lE: */
    alter.push_back(new _eLABEL(lE));
    
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
        alter.push_back(seq[line]);
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
    alter.push_back(new _eDIRECT(new _eVAR(ret), R3));
    /* -> goto lD */
    alter.push_back(new _eGOTO(lD));
    /* -> lEnd: */
    alter.push_back(new _eLABEL(lEnd));
    /* -> return ret */
    alter.push_back(new _eRET(new _eVAR(ret)));

    cerr << this_func << " is re-written as a bit-prob function" << endl;
    // for (auto stmt: alter) stmt->Dump(); fflush(stdout);
    seq = alter; n = seq.size(); return true;
}