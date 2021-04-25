#include <bits/stdc++.h>
#include "sysyAST.hpp"
#include "sysyLUT.hpp"
using namespace std;

extern TokenManager *tokenManager;
extern vector<nodeAST *> globalInitList;

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
    bufferStmt("\t" + t + " = " + symbol() + aop);
    return t;
}
string _BINARY_OP::atomize() {
    string alop = lop->atomize();
    string arop = rop->atomize();
    string t = tokenManager->newt();
    bufferStmt("\t" + t + " = " + alop + " " + symbol() + " " + arop);
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

        bufferStmt("\t" + t1 + " = " + ret + " * " + to_string(dim[k]));
        bufferStmt("\t" + t2 + " = " + t1 + " + " + nxt);
        ret = t2;
        cur = (_ADDR_LIST *) (cur->tail);
    }
    assert(cur == NULL);
    string t = tokenManager->newt();
    bufferStmt("\t" + t + " = " + ret + " * 4");
    return t;
}
string _ARRAY_ITEM::atomize() {
    /* potential optimization for constant array */
    string t = tokenManager->newt();
    string lval = param->atomize(token);
    bufferStmt("\t" + t + " = " + tokenManager->getEeyore(token) + "[" + lval + "]");
    return t;
}
string _FUNC_CALL::atomize() {
    string t = tokenManager->newt();
    if (param) param->pass();
        /* param = NULL when func() */
    bufferStmt("\t" + t + " = call f_" + token);
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
    return tokenManager->getEeyore(token) + "[" + lval + "]";
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
        bufferStmt("\tparam " + p);
}

/* ================================================= */
/* instantialize                                     */
/*      - eval constant var/arr                      */
/*      - by tokenManager, must after construction   */
/* ================================================= */
void _DEF_CONST_VAR::instantialize() {
    if (inits) tokenManager->instantialize(token);
}
void _DEF_CONST_ARR::instantialize() {
    if (inits) tokenManager->instantialize(token);
}
/* ================================================= */
/* initialize                                        */
/*      - initialize when construction               */
/*      - global var ought to be initialized in main */
/* ================================================= */
void _DEF_VAR::initialize() {
    if (inits) tokenManager->initialize(token, true);
}
void _DEF_ARR::initialize() {
    if (inits) tokenManager->initialize(token, false);
}

/* ================================================= */
/* traverse                                          */
/*      -                                            */
/*      -                                            */
/* ================================================= */
void _PROGRAM::traverse(string cp, bool glb) {
    if (program)
        program->traverse(cp, glb);
    printStmt();
}
void _STMT_SEQ::traverse(string cp, bool glb) {
    if (head != NULL) head->traverse(cp, glb);
    if (tail != NULL) tail->traverse(cp, glb);
}
void _IF::traverse(string cp, bool glb) {
    string c = cond->atomize();
    cp = tokenManager->newl();
    bufferStmt("\tif " + c + " == 0 goto " + cp);
    body->traverse(cp, glb);
    bufferStmt(cp + ":");
}
void _IF_ELSE::traverse(string cp, bool glb) {
    string cp1 = tokenManager->newl();
    string cp2 = tokenManager->newl();
    string c = cond->atomize();
    bufferStmt("\tif " + c + " == 0 goto " + cp1);
    body->traverse(cp2, glb);
    bufferStmt("\tgoto " + cp2);
    bufferStmt(cp1 + ":");
    ebody->traverse(cp2, glb);
    bufferStmt(cp2 + ":");
}
void _WHILE::traverse(string cp, bool glb) {
    string cp1 = tokenManager->newl();
    string cp2 = tokenManager->newl();
    bufferStmt(cp1 + ":");
        /* must set chk-point first, then atomize cond */
    string c = cond->atomize();
    bufferStmt("\tif " + c + " == 0 goto " + cp2);
    body->traverse(cp2, glb);
    bufferStmt("\tgoto " + cp1);
    bufferStmt(cp2 + ":");
}
void _RETURN_VOID::traverse(string cp, bool glb) {
    bufferStmt("\treturn");
}
void _RETURN_EXPR::traverse(string cp, bool glb) {
    string t = expr->atomize();
    bufferStmt("\treturn " + t);
}
void _CONTINUE::traverse(string cp, bool glb) {
    bufferStmt("\tgoto " + cp);
}
void _ASSIGN::traverse(string cp, bool glb) {
    string lval = lop->lvalize();
    string rval = rop->atomize();
    bufferStmt("\t" + lval + " = " + rval);
}
void _BLOCK::traverse(string cp, bool glb) {
    tokenManager->ascend();
    block->traverse(cp, glb);
    tokenManager->descend();
}
void _PARAM_LIST::traverse(string cp, bool glb) {
    if (head) head->traverse(cp, glb);
    if (tail) tail->traverse(cp, glb);
}
void _FUNC::traverse(string cp, bool glb) {
    int c = param ? param->countp() : 0;
    if (param) param->traverse(cp, glb);
        /* param = NULL when func() {} */
    printDecl("f_" + token + " [" + to_string(c) + "]");
    if (token == "main") {
        /* initialization of global var */
        for (auto glb_var : globalInitList)
            glb_var->initialize();
    }
    body->traverse(cp, false);
    printStmt();
    printDecl("end f_" + token);
}
void _FUNC_CALL::traverse(string cp, bool glb) {
    if (param) param->pass();
        /* param = NULL when func() */
    bufferStmt("\tcall f_" + token);
}
void _DEF_VAR::traverse(string cp, bool glb) {
    vector<int> dim {};
    dataDescript *dd = new dataDescript(token, dim, (_TREE *)inits);
    tokenManager->insert(dd);
    instantialize();
    if (!glb)
        initialize();
    else
        globalInitList.push_back(this);
}
void _PARAM_VAR::traverse(string cp, bool glb) {
    vector<int> dim {};
    dataDescript *dd = new dataDescript(token, dim, (_TREE *)inits);
    tokenManager->insert(dd, true);
}
void _DEF_ARR::traverse(string cp, bool glb) {
    vector<int> dim {};
    addr->vectorize(dim);
    dataDescript *dd = new dataDescript(token, dim, (_TREE *)inits);
    tokenManager->insert(dd);
    instantialize();
    if (!glb)
        initialize();
    else
        globalInitList.push_back(this);
}
void _PARAM_ARR::traverse(string cp, bool glb) {
    vector<int> dim {0};
    if (addr) addr->vectorize(dim);
        /* addr = NULL when token[] */
    dataDescript *dd = new dataDescript(token, dim, (_TREE *)inits);
    tokenManager->insert(dd, true);
}