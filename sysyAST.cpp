#include <bits/stdc++.h>
#include "sysyAST.hpp"
#include "sysyLUT.hpp"
using namespace std;

extern TokenManager *tokenManager;
extern FuncManager *funcManager;
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
/*      - AND/ OR is special                         */
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
string _AND::atomize() {
    string t = tokenManager->newt();
    bufferStmt("\t" + t + " = 0");
    string alop = lop->atomize();
    string cp = tokenManager->newl();
    bufferStmt("\tif " + alop + " == 0 goto " + cp);
    string arop = rop->atomize();
    bufferStmt("\tif " + arop + " == 0 goto " + cp);
    bufferStmt("\t" + t + " = 1");
    bufferStmt(cp + ":");
    return t;
}
string _OR::atomize() {
    string t = tokenManager->newt();
    bufferStmt("\t" + t + " = 1");
    string alop = lop->atomize();
    string cp = tokenManager->newl();
    bufferStmt("\tif " + alop + " != 0 goto " + cp);
    string arop = rop->atomize();
    bufferStmt("\tif " + arop + " != 0 goto " + cp);
    bufferStmt("\t" + t + " = 0");
    bufferStmt(cp + ":");
    return t;
}
string _ADDR_LIST::atomize(string token) {
    vector<int> &dim = tokenManager->getDim(token);
    assert(head != NULL);
    string ret = head->atomize();
    _ADDR_LIST *cur = (_ADDR_LIST *)tail;
    for (int k = 1; k < dim.size(); k++)
    if (cur) {
        assert(cur->head);
        string nxt = cur->head->atomize();
        string t1 = tokenManager->newt();
        string t2 = tokenManager->newt();

        bufferStmt("\t" + t1 + " = " + ret + " * " + to_string(dim[k]));
        bufferStmt("\t" + t2 + " = " + t1 + " + " + nxt);
        ret = t2;
        cur = (_ADDR_LIST *) (cur->tail);
    }
    else {
        string t1 = tokenManager->newt();
        bufferStmt("\t" + t1 + " = " + ret + " * " + to_string(dim[k]));
        ret = t1;
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
    if (param) pass();
        /* if could be deleted */
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
void _FUNC_CALL::pass() {
    vector<string> call_list {};
    _CALL_LIST *cur = (_CALL_LIST *) param;
    vector<int> &isvars = funcManager->query(token);
    
    for (auto isvar: isvars) {
        assert(cur->head != NULL);
        if (isvar || cur->head->var())
            call_list.push_back(cur->head->atomize());
        else {
            /* ought to compute addr */
            _ARRAY_ITEM *ai = (_ARRAY_ITEM *) (cur->head);
            string t = tokenManager->newt();
            string addr = ai->param->atomize(ai->token);
                /* ai->token, token must be passed!!
                    no definition on _ADDR_LIST->atomize(void) */
            bufferStmt("\t" + t + " = " + ai->token + " + " + addr);
            call_list.push_back(t);
        }
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
void _PROGRAM::traverse(string ctn, string brk, bool glb) {
    if (program)
        program->traverse(ctn, brk, glb);
    printStmt();
}
void _STMT_SEQ::traverse(string ctn, string brk, bool glb) {
    if (head != NULL) head->traverse(ctn, brk, glb);
    if (tail != NULL) tail->traverse(ctn, brk, glb);
}
void _IF::traverse(string ctn, string brk, bool glb) {
    string c = cond->atomize();
    string cp = tokenManager->newl();
    bufferStmt("\tif " + c + " == 0 goto " + cp);
    body->traverse(ctn, brk, glb);
    bufferStmt(cp + ":");
}
void _IF_ELSE::traverse(string ctn, string brk, bool glb) {
    string cp1 = tokenManager->newl();
    string cp2 = tokenManager->newl();
    string c = cond->atomize();
    bufferStmt("\tif " + c + " == 0 goto " + cp1);
    body->traverse(ctn, brk, glb);
    bufferStmt("\tgoto " + cp2);
    bufferStmt(cp1 + ":");
    ebody->traverse(ctn, brk, glb);
    bufferStmt(cp2 + ":");
}
void _WHILE::traverse(string ctn, string brk, bool glb) {
    string cp1 = tokenManager->newl();
    string cp2 = tokenManager->newl();
    bufferStmt(cp1 + ":");
        /* must set chk-point first, then atomize cond */
    string c = cond->atomize();
    bufferStmt("\tif " + c + " == 0 goto " + cp2);
    body->traverse(cp1, cp2, glb);
    bufferStmt("\tgoto " + cp1);
    bufferStmt(cp2 + ":");
}
void _RETURN_VOID::traverse(string ctn, string brk, bool glb) {
    bufferStmt("\treturn");
}
void _RETURN_EXPR::traverse(string ctn, string brk, bool glb) {
    string t = expr->atomize();
    bufferStmt("\treturn " + t);
}
void _CONTINUE::traverse(string ctn, string brk, bool glb) {
    bufferStmt("\tgoto " + ctn);
}
void _BREAK::traverse(string ctn, string brk, bool glb) {
    bufferStmt("\tgoto " + brk);
}
void _ASSIGN::traverse(string ctn, string brk, bool glb) {
    string lval = lop->lvalize();
    string rval = rop->atomize();
    bufferStmt("\t" + lval + " = " + rval);
}
void _BLOCK::traverse(string ctn, string brk, bool glb) {
    tokenManager->ascend();
    if (block) block->traverse(ctn, brk, glb);
        /* possible occassions: block body is empty */
    tokenManager->descend();
}
void _PARAM_LIST::traverse(string ctn, string brk, bool glb) {
    if (head) head->traverse(ctn, brk, glb);
    if (tail) tail->traverse(ctn, brk, glb);
}
void _FUNC::traverse(string ctn, string brk, bool glb) {
    tokenManager->ascend();
    int p = funcManager->insert(token, param);
        /* bool vector: is each param an var(T)/array(F) */
    if (param) param->traverse(ctn, brk, glb);
        /* param = NULL when func() {} */
    printDecl("f_" + token + " [" + to_string(p) + "]");
    if (token == "main") {
        /* initialization of global var */
        for (auto glb_var : globalInitList)
            glb_var->initialize();
    }
    assert(body != NULL);
    body->traverse(ctn, brk, false);
    if (isvoid) bufferStmt("\treturn");
    printStmt();
    printDecl("end f_" + token);
    tokenManager->descend();
}
void _FUNC_CALL::traverse(string ctn, string brk, bool glb) {
    if (param) pass();
        /* param = NULL when func() */
    bufferStmt("\tcall f_" + token);
}
void _DEF_VAR::traverse(string ctn, string brk, bool glb) {
    vector<int> dim {};
    dataDescript *dd = new dataDescript(token, dim, (_TREE *)inits);
    tokenManager->insert(dd);
    instantialize();
    if (!glb)
        initialize();
    else
        globalInitList.push_back(this);
}
void _PARAM_VAR::traverse(string ctn, string brk, bool glb) {
    vector<int> dim {};
    dataDescript *dd = new dataDescript(token, dim, (_TREE *)inits);
    tokenManager->insert(dd, true);
}
void _DEF_ARR::traverse(string ctn, string brk, bool glb) {
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
void _PARAM_ARR::traverse(string ctn, string brk, bool glb) {
    vector<int> dim {0};
    if (addr) addr->vectorize(dim);
        /* addr = NULL when token[] */
    dataDescript *dd = new dataDescript(token, dim, (_TREE *)inits);
    tokenManager->insert(dd, true);
}