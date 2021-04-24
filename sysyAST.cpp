#include <bits/stdc++.h>
#include "sysyAST.hpp"
#include "sysyLUT.hpp"
using namespace std;

extern TokenManager *tokenManager;

/* ================================================= */
/* eval                                              */
/*      - most parts in .hpp                         */
/*      - here using tokenManager                    */
/* ================================================= */
int _VAR::eval() {
    return tokenManager->query(token);
}
int _ARRAY_ITEM::eval() {
    vector<int> addr{};
    param->vectorize(addr);
    return tokenManager->query(token, addr);
}

/* ================================================= */
/* atomize                                           */
/*      - reduce to var                              */
/*      - including function call & array item       */
/* ================================================= */
string _UNARY_OP::atomize() {
    string aop = op->atomize();
    string t = tokenManager->newt();
    printf("\t%s = %s %s\n", t.c_str(), symbol().c_str(), aop.c_str());
    return t;
}
string _BINARY_OP::atomize() {
    string alop = lop->atomize();
    string arop = rop->atomize();
    string t = tokenManager->newt();
    printf("\t%s = %s %s %s\n", t.c_str(), alop.c_str(), symbol().c_str(), arop.c_str());
    return t;
}
string _ADDR_LIST::atomize(string token) {
    vector<int> &dim = tokenManager->getDim(token);
    assert(head != NULL);
    string ret = head->atomize();
    _ADDR_LIST *cur = (_ADDR_LIST *)tail;
    for (int k = 1; k < dim.size(); k++)
    {
        assert(cur && cur->head);
        string nxt = cur->head->atomize();
        string t1 = tokenManager->newt();
        string t2 = tokenManager->newt();
        printf("\t%s = %s * %d\n", t1.c_str(), ret.c_str(), dim[k]);
        printf("\t%s = %s + %s\n", t2.c_str(), t1.c_str(), nxt.c_str());
        ret = t2;
        cur = (_ADDR_LIST *) (cur->tail);
    }
    assert(cur == NULL);
    string t = tokenManager->newt();
    printf("\t%s = %s * 4\n", t.c_str(), ret.c_str());
    return t;
}
string _ARRAY_ITEM::atomize() {
    string t = tokenManager->newt();
    string lval = param->atomize(token);
    printf("\t%s = %s [ %s ]\n", t.c_str(), tokenManager->getEeyore(token).c_str(), lval.c_str());
    return t;
}
string _FUNC_CALL::atomize() {
    string t = tokenManager->newt();
    if (param) param->pass();
        /* param = NULL when func() */
    printf("\t%s = call f_%s\n", t.c_str(), token.c_str());
    return t;
}
string _VAR::atomize() {
    return tokenManager->getEeyore(token); /* translate name */
}

/* ================================================= */
/* lvalize                                           */
/*      - reduce to lval                             */
/*      - var, array item                            */
/* ================================================= */
string _ARRAY_ITEM::lvalize() {
    string lval = param->atomize(token);
    return tokenManager->getEeyore(token) + " [ " + lval + " ] ";
}
string _VAR::lvalize() {
    return tokenManager->getEeyore(token);
}

/* ================================================= */
/* pass                                              */
/*      - pass var within function call              */
/*      - using param, closely followed by calling   */
/* ================================================= */
void _CALL_LIST::pass() {
    vector<string> call_list {};
    _CALL_LIST *cur = this;
    while (cur != NULL) {
        call_list.push_back(cur->head->atomize());
        cur = (_CALL_LIST *) (cur->tail);
    }
    for (auto p: call_list)
        printf("\tparam %s\n", p.c_str());
}

/* ================================================= */
/* traverse                                          */
/*      -                                            */
/*      -                                            */
/* ================================================= */
void _STMT_SEQ::traverse(string cp) {
    if (head != NULL) head->traverse(cp);
    if (tail != NULL) tail->traverse(cp);
}
void _IF::traverse(string cp) {
    string c = cond->atomize();
    cp = tokenManager->newl();
    printf("\tif %s == 0 goto %s\n", c.c_str(), cp.c_str());
    body->traverse(cp.c_str());
    printf("%s:\n", cp.c_str());
}
void _IF_THEN::traverse(string cp) {
    string cp1 = tokenManager->newl();
    string cp2 = tokenManager->newl();
    string c = cond->atomize();
    printf("\tif %s == 0 goto %s\n", c.c_str(), cp1.c_str());
    body->traverse(cp2);
    printf("\tgoto %s\n", cp2.c_str());
    printf("%s:\n", cp1.c_str());
    ebody->traverse(cp2);
    printf("%s:\n", cp2.c_str());
}
void _WHILE::traverse(string cp) {
    string cp1 = tokenManager->newl();
    string cp2 = tokenManager->newl();
    printf("%s:\n", cp1.c_str());
        /* must set chk-point first, then atomize cond */
    string c = cond->atomize();
    printf("\tif %s == 0 goto %s\n", c.c_str(), cp2.c_str());
    body->traverse(cp2);
    printf("\tgoto %s\n", cp1.c_str());
    printf("%s:\n", cp2.c_str());
}
void _RETURN_VOID::traverse(string cp) {
    printf("\treturn\n");
}
void _RETURN_EXPR::traverse(string cp) {
    string t = expr->atomize();
    printf("\treturn %s\n", t.c_str());
}
void _CONTINUE::traverse(string cp) {
    printf("\tgoto %s\n", cp.c_str());
}
void _ASSIGN::traverse(string cp) {
    string lval = lop->lvalize();
    string rval = rop->atomize();
    printf("\t%s = %s\n", lval.c_str(), rval.c_str());
}
void _BLOCK::traverse(string cp) {
    tokenManager->ascend();
    block->traverse(cp);
    tokenManager->descend();
}
void _FUNC::traverse(string cp) {
    int c = param ? param->countp() : 0;
    if (param) param->traverse(cp);
        /* param = NULL when func() {} */
    printf("begin f_%s [ %d ]\n", token.c_str(), c);
    body->traverse(cp);
    printf("end f_%s\n", token.c_str());
}
void _FUNC_CALL::traverse(string cp) {
    if (param) param->pass();
        /* param = NULL when func() */
    printf("\tcall f_%s\n", token.c_str());
}
void _DEF_VAR::traverse(string cp) {
    vector<int> dim {};
    dataDescript *dd = new dataDescript(token, dim, (_TREE *)inits);
    tokenManager->insert(dd);
}
void _PARAM_VAR::traverse(string cp) {
    vector<int> dim {};
    dataDescript *dd = new dataDescript(token, dim, (_TREE *)inits);
    tokenManager->insert(dd, true);
}
void _DEF_ARR::traverse(string cp) {
    vector<int> dim {};
    addr->vectorize(dim);
    dataDescript *dd = new dataDescript(token, dim, (_TREE *)inits);
    tokenManager->insert(dd);
}
void _PARAM_ARR::traverse(string cp) {
    vector<int> dim {0};
    if (addr) addr->vectorize(dim);
        /* addr = NULL when token[] */
    dataDescript *dd = new dataDescript(token, dim, (_TREE *)inits);
    tokenManager->insert(dd, true);
}