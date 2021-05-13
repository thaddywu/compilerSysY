#ifndef defs_hpp
#include "defs.hpp"
#define defs_hpp
#endif
using namespace std;

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
eeyoreAST* _UNARY_OP::atomize() {
    eeyoreAST* aop = op->atomize();
    eeyoreAST* t = new _eVAR(tokenManager->newt());
    eeyoreStmt(new _eUNARY(t, symbol(), aop));
    return t;
}
eeyoreAST* _BINARY_OP::atomize() {
    eeyoreAST* alop = lop->atomize();
    eeyoreAST* arop = rop->atomize();
    eeyoreAST* t = new _eVAR(tokenManager->newt());
    eeyoreStmt(new _eBINARY(t, alop, symbol(), arop));
    return t;
}
eeyoreAST* _AND::atomize() {
    eeyoreAST* t = new _eVAR(tokenManager->newt());
    eeyoreStmt(new _eDIRECT(t, new _eNUM(0)));
    eeyoreAST* alop = lop->atomize();
    string cp = tokenManager->newl();
    eeyoreStmt(new _eIFGOTO(alop, "==", new _eNUM(0), cp));
    eeyoreAST* arop = rop->atomize();
    eeyoreStmt(new _eIFGOTO(arop, "==", new _eNUM(0), cp));
    eeyoreStmt(new _eDIRECT(t, new _eNUM(1)));
    eeyoreStmt(new _eLABEL(cp));
    return t;
}
eeyoreAST* _OR::atomize() {
    eeyoreAST* t = new _eVAR(tokenManager->newt());
    eeyoreStmt(new _eDIRECT(t, new _eNUM(1)));
    eeyoreAST* alop = lop->atomize();
    string cp = tokenManager->newl();
    eeyoreStmt(new _eIFGOTO(alop, "!=", new _eNUM(0), cp));
    eeyoreAST* arop = rop->atomize();
    eeyoreStmt(new _eIFGOTO(arop, "!=", new _eNUM(0), cp));
    eeyoreStmt(new _eDIRECT(t, new _eNUM(0)));
    eeyoreStmt(new _eLABEL(cp));
    return t;
}
eeyoreAST* _ADDR_LIST::atomize(string token) {
    vector<int> &dim = tokenManager->getDim(token);
    assert(head != NULL);
    eeyoreAST* ret = head->atomize();
    _ADDR_LIST *cur = (_ADDR_LIST *)tail;
    for (int k = 1; k < dim.size(); k++)
    if (cur) {
        assert(cur->head);
        eeyoreAST* nxt = cur->head->atomize();
        eeyoreAST* t1 = new _eVAR(tokenManager->newt());
        eeyoreAST* t2 = new _eVAR(tokenManager->newt());

        eeyoreStmt(new _eBINARY(t1, ret, "*", new _eNUM(dim[k])));
        eeyoreStmt(new _eBINARY(t2, t1, "+", nxt));
        ret = t2;
        cur = (_ADDR_LIST *) (cur->tail);
    }
    else {
        eeyoreAST* t1 = new _eVAR(tokenManager->newt());
        eeyoreStmt(new _eBINARY(t1, ret, "*", new _eNUM(dim[k])));
        ret = t1;
    }
    assert(cur == NULL);
    eeyoreAST* t = new _eVAR(tokenManager->newt());
    eeyoreStmt(new _eBINARY(t, ret, "*", new _eNUM(4)));
    return t;
}
eeyoreAST* _ARRAY_ITEM::atomize() {
    /* potential optimization for constant array */
    eeyoreAST* t = new _eVAR(tokenManager->newt());
    eeyoreAST* lval = param->atomize(token);
    eeyoreStmt(new _eSEEK(t, tokenManager->getEeyore(token), lval));
    return t;
}
eeyoreAST* _FUNC_CALL::atomize() {
    eeyoreAST* t = new _eVAR(tokenManager->newt());
    if (param) pass();
    eeyoreStmt(new _eFUNCRET(t, "f_" + token));
    return t;
}
eeyoreAST* _VAR::atomize() {
    return new _eVAR(tokenManager->getEeyore(token)); /* atomize name */
}
eeyoreAST* _INTEGER::atomize() {
    return new _eNUM(num);
}

/* ================================================= */
/* pass                                              */
/*      - pass var within function call              */
/*      - using param, closely followed by calling   */
/* ================================================= */
void _FUNC_CALL::pass() {
    vector<eeyoreAST *> call_list {};
    _CALL_LIST *cur = (_CALL_LIST *) param;
    vector<int> &isvars = funcManager->query(token);
    
    for (auto isvar: isvars) {
        assert(cur->head != NULL);
        if (isvar || cur->head->isvar())
            call_list.push_back(cur->head->atomize());
        else {
            /* ought to compute addr */
            _ARRAY_ITEM *ai = (_ARRAY_ITEM *) (cur->head);
            eeyoreAST* t = new _eVAR(tokenManager->newt());
            eeyoreAST* addr = ai->param->atomize(ai->token);
                /* ai->token, token must be passed!!
                    no definition on _ADDR_LIST->atomize(void) */
            eeyoreStmt(new _eBINARY(t, new _eVAR(ai->token), "+", addr));
            call_list.push_back(t);
        }
        cur = (_CALL_LIST *) (cur->tail);
    }
    for (auto p: call_list)
        eeyoreStmt(new _ePARAM(p));
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
/* translate                                          */
/*      -  translate AST, and do the conversion.      */
/*      - ctn: continue to where                     */
/*      - brk: break to where                        */
/*      - glb: in global environment?                */
/* ================================================= */
void _PROGRAM::translate(string ctn, string brk, bool glb) {
    if (program)
        program->translate(ctn, brk, glb);
    eeyoreList.insert(eeyoreList.end(), eeyoreDeclList.begin(), eeyoreDeclList.end());
    eeyoreDeclList.clear();
    /* print decls in buffer into the entire code */
    eeyoreRoot = new _eSEQ(eeyoreList);
}
void _EXPR::translate(string ctn, string brk, bool glb) {
    /* do nothing , expr ;*/
}
void _STMT_SEQ::translate(string ctn, string brk, bool glb) {
    if (head != NULL) head->translate(ctn, brk, glb);
    if (tail != NULL) tail->translate(ctn, brk, glb);
}
void _IF::translate(string ctn, string brk, bool glb) {
    eeyoreAST* c = cond->atomize();
    string cp = tokenManager->newl();
    eeyoreStmt(new _eIFGOTO(c, "==", new _eNUM(0), cp));
    body->translate(ctn, brk, glb);
    eeyoreStmt(new _eLABEL(cp));
}
void _IF_ELSE::translate(string ctn, string brk, bool glb) {
    string cp1 = tokenManager->newl();
    string cp2 = tokenManager->newl();
    eeyoreAST* c = cond->atomize();
    eeyoreStmt(new _eIFGOTO(c, "==", new _eNUM(0), cp1));
    body->translate(ctn, brk, glb);
    eeyoreStmt(new _eGOTO(cp2));
    eeyoreStmt(new _eLABEL(cp1));
    ebody->translate(ctn, brk, glb);
    eeyoreStmt(new _eLABEL(cp2));
}
void _WHILE::translate(string ctn, string brk, bool glb) {
    string cp1 = tokenManager->newl();
    string cp2 = tokenManager->newl();
    eeyoreStmt(new _eLABEL(cp1));
        /* must set label first, then atomize cond */
    eeyoreAST* c = cond->atomize();
    eeyoreStmt(new _eIFGOTO(c, "==", new _eNUM(0), cp2));
    body->translate(cp1, cp2, glb);
    eeyoreStmt(new _eGOTO(cp1));
    eeyoreStmt(new _eLABEL(cp2));
}
void _RETURN_VOID::translate(string ctn, string brk, bool glb) {
    eeyoreStmt(new _eRETVOID());
}
void _RETURN_EXPR::translate(string ctn, string brk, bool glb) {
    eeyoreAST* t = expr->atomize();
    eeyoreStmt(new _eRET(t));
}
void _CONTINUE::translate(string ctn, string brk, bool glb) {
    eeyoreStmt(new _eGOTO(ctn));
}
void _BREAK::translate(string ctn, string brk, bool glb) {
    eeyoreStmt(new _eGOTO(brk));
}
void _ASSIGN::translate(string ctn, string brk, bool glb) {
    eeyoreAST* rval = rop->atomize();
    if (lop->isvar()) {
        _VAR *var = (_VAR *)lop;
        eeyoreStmt(new _eDIRECT(new _eVAR(tokenManager->getEeyore(var->token)), rval));
    }
    else {
        _ARRAY_ITEM *ai = (_ARRAY_ITEM *)lop;
        eeyoreAST* param = ai->param->atomize(ai->token);
            /* remember to write ->atomize(token) !!! */
        eeyoreStmt(new _eSAVE(tokenManager->getEeyore(ai->token), param, rval));
    }
}
void _BLOCK::translate(string ctn, string brk, bool glb) {
    tokenManager->newEnviron();
    if (block) block->translate(ctn, brk, glb);
        /* possible occassions: block body is empty */
    tokenManager->deleteEnviron();
}
void _PARAM_LIST::translate(string ctn, string brk, bool glb) {
    if (head) head->translate(ctn, brk, glb);
    if (tail) tail->translate(ctn, brk, glb);
}
void _FUNC::translate(string ctn, string brk, bool glb) {
    eeyoreList.insert(eeyoreList.end(), eeyoreDeclList.begin(), eeyoreDeclList.end());
    eeyoreDeclList.clear();
    /* print decls in buffer into the entire code */
    tokenManager->newEnviron();
    int p = funcManager->insert(token, param);
        /* bool vector: is each param an var(T)/array(F) */
    if (param) param->translate(ctn, brk, glb);
        /* param = NULL in case func() {} */
    if (token == "main") {
        /* initialization of global var */
        for (auto glb_var : globalInitList)
            glb_var->initialize();
    }
    assert(body != NULL);
    body->translate(ctn, brk, false);
    if (isvoid) eeyoreStmt(new _eRETVOID());
    eeyoreDeclList.insert(eeyoreDeclList.end(), eeyoreStmtList.begin(), eeyoreStmtList.end());
    eeyoreStmtList.clear();
    /* concantenate decls with normal codes */
    eeyoreAST *func = new _eFUNC("f_" + token, p, new _eSEQ(eeyoreDeclList));
    eeyoreDeclList.clear();
    /* print codes buffer into the structure of function */
    eeyoreDecl(func);
    tokenManager->deleteEnviron();
}
void _FUNC_CALL::translate(string ctn, string brk, bool glb) {
    if (param) pass();
        /* param = NULL in case func() */
    eeyoreStmt(new _eCALL("f_" + token));
}
void _DEF_VAR::translate(string ctn, string brk, bool glb) {
    vector<int> dim {};
    dataDescript *dd = new dataDescript(token, dim, (_TREE *)inits);
    tokenManager->insert(dd);
    instantialize();
    if (!glb)
        initialize();
    else /* delay global variables' initialization, do it in main */
        globalInitList.push_back(this);
}
void _PARAM_VAR::translate(string ctn, string brk, bool glb) {
    vector<int> dim {};
    dataDescript *dd = new dataDescript(token, dim, (_TREE *)inits);
    tokenManager->insert(dd, true);
}
void _DEF_ARR::translate(string ctn, string brk, bool glb) {
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
void _PARAM_ARR::translate(string ctn, string brk, bool glb) {
    vector<int> dim {0};
    if (addr) addr->vectorize(dim);
        /* addr = NULL in case token[] */
    dataDescript *dd = new dataDescript(token, dim, (_TREE *)inits);
    tokenManager->insert(dd, true);
}