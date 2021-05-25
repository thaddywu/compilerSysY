#ifndef defs_hpp
#include "defs.hpp"
#define defs_hpp
#endif
using namespace std;

/* ================================================= */
/* eval                                              */
/*      - most parts in .hpp                         */
/*      - here using varManager                    */
/* ================================================= */
int _VAR::eval() {
    return varManager->query(name);
}
int _ARRAY_ITEM::eval() {
    vector<int> addr{};
    param->vectorize(addr);
    return varManager->query(name, addr);
}

/* ================================================= */
/* atomize                                           */
/*      - target is the defualt target               */
/*      - if target is NULL, temporary'll var be used*/
/*      - AND/ OR is special                         */
/* ================================================= */
eeyoreAST* _UNARY_OP::atomize(eeyoreAST* target) {
    eeyoreAST* aop = op->atomize(NULL);
    eeyoreAST* t = target ? target : new _eVAR(varManager->newt());
    eeyoreStmt(new _eUNARY(t, symbol(), aop));
    return t;
}
eeyoreAST* _BINARY_OP::atomize(eeyoreAST* target) {
    eeyoreAST* alop = lop->atomize(NULL);
    eeyoreAST* arop = rop->atomize(NULL);
    eeyoreAST* t = target ? target : new _eVAR(varManager->newt());
    eeyoreStmt(new _eBINARY(t, alop, symbol(), arop));
    return t;
}
eeyoreAST* _AND::atomize(eeyoreAST* target) {
    eeyoreAST* t = target ? target : new _eVAR(varManager->newt());
    eeyoreStmt(new _eDIRECT(t, new _eNUM(0)));
    eeyoreAST* alop = lop->atomize(NULL);
    string cp = varManager->newl();
    eeyoreStmt(new _eIFGOTO(alop, "==", new _eNUM(0), cp));
    eeyoreAST* arop = rop->atomize(NULL);
    eeyoreStmt(new _eIFGOTO(arop, "==", new _eNUM(0), cp));
    eeyoreStmt(new _eDIRECT(t, new _eNUM(1)));
    eeyoreStmt(new _eLABEL(cp));
    return t;
}
eeyoreAST* _OR::atomize(eeyoreAST* target) {
    eeyoreAST* t = target ? target : new _eVAR(varManager->newt());
    eeyoreStmt(new _eDIRECT(t, new _eNUM(1)));
    eeyoreAST* alop = lop->atomize(NULL);
    string cp = varManager->newl();
    eeyoreStmt(new _eIFGOTO(alop, "!=", new _eNUM(0), cp));
    eeyoreAST* arop = rop->atomize(NULL);
    eeyoreStmt(new _eIFGOTO(arop, "!=", new _eNUM(0), cp));
    eeyoreStmt(new _eDIRECT(t, new _eNUM(0)));
    eeyoreStmt(new _eLABEL(cp));
    return t;
}
eeyoreAST* _ADDR_LIST::atomize(string name, eeyoreAST* target) {
    vector<int> &shape = varManager->getshape(name);
    assert(head != NULL);
    eeyoreAST* ret = head->atomize(NULL);
    _ADDR_LIST *cur = (_ADDR_LIST *)tail;
    for (int k = 1; k < shape.size(); k++)
    if (cur) {
        assert(cur->head);
        eeyoreAST* nxt = cur->head->atomize(NULL);
        eeyoreAST* t1 = new _eVAR(varManager->newt());
        eeyoreAST* t2 = new _eVAR(varManager->newt());

        eeyoreStmt(new _eBINARY(t1, ret, "*", new _eNUM(shape[k])));
        eeyoreStmt(new _eBINARY(t2, t1, "+", nxt));
        ret = t2;
        cur = (_ADDR_LIST *) (cur->tail);
    }
    else {
        eeyoreAST* t1 = new _eVAR(varManager->newt());
        eeyoreStmt(new _eBINARY(t1, ret, "*", new _eNUM(shape[k])));
        ret = t1;
    }
    assert(cur == NULL);
    eeyoreAST* t = target ? target : new _eVAR(varManager->newt());
    eeyoreStmt(new _eBINARY(t, ret, "*", new _eNUM(4)));
    return t;
}
eeyoreAST* _ARRAY_ITEM::atomize(eeyoreAST* target) {
    /* potential optimization for constant array */
    eeyoreAST* t = target ? target : new _eVAR(varManager->newt());
    eeyoreAST* lval = param->atomize(name, NULL);
    eeyoreStmt(new _eSEEK(t, varManager->getEeyore(name), lval));
    return t;
}
eeyoreAST* _FUNC_CALL::atomize(eeyoreAST* target) {
    eeyoreAST* t = target ? target : new _eVAR(varManager->newt());
    if (param) pass();
    eeyoreStmt(new _eFUNCRET(t, name));
    return t;
}
eeyoreAST* _VAR::atomize(eeyoreAST* target) {
    return new _eVAR(varManager->getEeyore(name)); /* atomize name */
}
eeyoreAST* _INTEGER::atomize(eeyoreAST* target) {
    return new _eNUM(num);
}

/* ================================================= */
/* cjump                                             */
/*      - conditional jump to l                      */
/*      - neg: jump when condition fails             */
/* ================================================= */
void _EXPR::cjump(string l, bool neg) {
    eeyoreAST* c = this->atomize(NULL);
    eeyoreStmt(new _eIFGOTO(c, neg ? "==" : "!=", new _eNUM(0), l));
}
void _UNARY_OP::cjump(string l, bool neg) {
    eeyoreAST* c = op->atomize(NULL);
    if (symbol() == "!") neg = !neg;
    eeyoreStmt(new _eIFGOTO(c, neg ? "==" : "!=", new _eNUM(0), l));
}
void _BINARY_OP::cjump(string l, bool neg) {
    if (islogicop(symbol())) {
        eeyoreAST* alop = lop->atomize(NULL);
        eeyoreAST* arop = rop->atomize(NULL);
        string _op = neg ? neg_logicop(symbol()) : symbol();
        eeyoreStmt(new _eIFGOTO(alop, _op, arop, l));
    }
    else {
        eeyoreAST* c = this->atomize(NULL);
        eeyoreStmt(new _eIFGOTO(c, neg ? "==" : "!=", new _eNUM(0), l));
    }
}

/* ================================================= */
/* pass                                              */
/*      - pass var within function call              */
/*      - using param, closely followed by calling   */
/* ================================================= */
void _FUNC_CALL::pass() {
    vector<eeyoreAST *> call_list {};
    _CALL_LIST *cur = (_CALL_LIST *) param;
    vector<int> &isvars = funcManager->query(name);
    
    for (auto isvar: isvars) {
        assert(cur->head != NULL);
        if (isvar || cur->head->isvar())
            call_list.push_back(cur->head->atomize(NULL));
        else {
            /* ought to compute addr */
            _ARRAY_ITEM *ai = (_ARRAY_ITEM *) (cur->head);
            eeyoreAST* t = new _eVAR(varManager->newt());
            eeyoreAST* addr = ai->param->atomize(ai->name, NULL);
                /* ai->name, name must be passed!!
                    no definition on _ADDR_LIST->atomize(void) */
            eeyoreStmt(new _eBINARY(t, new _eVAR(ai->name), "+", addr));
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
/*      - by varManager, must after construction   */
/* ================================================= */
void _DEF_CONST_VAR::instantialize() {
    varManager->instantialize(name);
}
void _DEF_CONST_ARR::instantialize() {
    varManager->instantialize(name);
}
/* ================================================= */
/* initialize                                        */
/*      - initialize when construction               */
/*      - global var ought to be initialized in main */
/* ================================================= */
void _DEF_CONST_VAR::initialize(bool glb) {
    assert(inits != NULL);
    varManager->initialize(name, true, true);
}
void _DEF_CONST_ARR::initialize(bool glb) {
    assert(inits != NULL);
    varManager->initialize(name, false, true);
}
void _DEF_VAR::initialize(bool glb) {
    bool zero_pad = !glb && (inits != NULL);
    varManager->initialize(name, true, zero_pad);
}
void _DEF_ARR::initialize(bool glb) {
    bool zero_pad = !glb && (inits != NULL);
    varManager->initialize(name, false, zero_pad);
}

/* ================================================= */
/* translate                                         */
/*      -  translate AST, and do the conversion.     */
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
    string l = varManager->newl();
    cond->cjump(l, true);
    body->translate(ctn, brk, glb);
    eeyoreStmt(new _eLABEL(l));
}
void _IF_ELSE::translate(string ctn, string brk, bool glb) {
    string l1 = varManager->newl();
    string l2 = varManager->newl();
    cond->cjump(l1, true);
    body->translate(ctn, brk, glb);
    eeyoreStmt(new _eGOTO(l2));
    eeyoreStmt(new _eLABEL(l1));
    ebody->translate(ctn, brk, glb);
    eeyoreStmt(new _eLABEL(l2));
}
void _WHILE::translate(string ctn, string brk, bool glb) {
    string l1 = varManager->newl();
    string l2 = varManager->newl();
    eeyoreStmt(new _eLABEL(l1));
        /* must set label first, then cjump */
    cond->cjump(l2, true);
    body->translate(l1, l2, glb);
    eeyoreStmt(new _eGOTO(l1));
    eeyoreStmt(new _eLABEL(l2));
}
void _RETURN_VOID::translate(string ctn, string brk, bool glb) {
    eeyoreStmt(new _eRETVOID());
}
void _RETURN_EXPR::translate(string ctn, string brk, bool glb) {
    eeyoreAST* t = expr->atomize(NULL);
    eeyoreStmt(new _eRET(t));
}
void _CONTINUE::translate(string ctn, string brk, bool glb) {
    eeyoreStmt(new _eGOTO(ctn));
}
void _BREAK::translate(string ctn, string brk, bool glb) {
    eeyoreStmt(new _eGOTO(brk));
}
void _ASSIGN::translate(string ctn, string brk, bool glb) {
    if (lop->isvar()) {
        _VAR *var = (_VAR *)lop;
        eeyoreAST* lval = new _eVAR(varManager->getEeyore(var->name));
        eeyoreAST* rval = rop->atomize(lval);
        if (lval != rval) /* optimization here: avoid redundant pass */
            eeyoreStmt(new _eDIRECT(lval, rval));
    }
    else {
        eeyoreAST* rval = rop->atomize(NULL);
        _ARRAY_ITEM *ai = (_ARRAY_ITEM *)lop;
        eeyoreAST* param = ai->param->atomize(ai->name, NULL);
            /* remember to write ->atomize(name) !!! */
        eeyoreStmt(new _eSAVE(varManager->getEeyore(ai->name), param, rval));
    }
}
void _BLOCK::translate(string ctn, string brk, bool glb) {
    varManager->newEnviron();
    if (block) block->translate(ctn, brk, glb);
        /* possible occassions: block body is empty */
    varManager->deleteEnviron();
}
void _PARAM_LIST::translate(string ctn, string brk, bool glb) {
    if (head) head->translate(ctn, brk, glb);
    if (tail) tail->translate(ctn, brk, glb);
}
void _FUNC::translate(string ctn, string brk, bool glb) {
    eeyoreList.insert(eeyoreList.end(), eeyoreDeclList.begin(), eeyoreDeclList.end());
    eeyoreDeclList.clear();
    /* print decls in buffer into the entire code */
    varManager->newEnviron();
    int p = funcManager->insert(name, param);
        /* bool vector: is each param an var(T)/array(F) */
    if (param) param->translate(ctn, brk, glb);
        /* param = NULL in case func() {} */
    if (name == "main") {
        /* initialization of global var */
        for (auto glb_var : globalInitList)
            glb_var->initialize(true);
    }
    assert(body != NULL);
    body->translate(ctn, brk, false);
    if (isvoid) eeyoreStmt(new _eRETVOID());
    // else eeyoreStmt(new _eRET(new _eNUM(0)));
    eeyoreDeclList.insert(eeyoreDeclList.end(), eeyoreStmtList.begin(), eeyoreStmtList.end());
    eeyoreStmtList.clear();
    /* concantenate decls with normal codes */
    eeyoreAST *func = new _eFUNC(name, p, new _eSEQ(eeyoreDeclList));
    eeyoreDeclList.clear();
    /* print codes buffer into the structure of function */
    eeyoreDecl(func);
    varManager->deleteEnviron();
}
void _FUNC_CALL::translate(string ctn, string brk, bool glb) {
    if (param) pass();
        /* param = NULL in case func() */
    eeyoreStmt(new _eCALL(name));
}
void _DEF_VAR::translate(string ctn, string brk, bool glb) {
    vector<int> shape {};
    dataDescript *dd = new dataDescript(name, shape, (_TREE *)inits);
    varManager->insert(dd);
    instantialize();
    if (!glb)
        initialize(glb);
    else /* delay global variables' initialization, do it in main */
        globalInitList.push_back(this);
}
void _PARAM_VAR::translate(string ctn, string brk, bool glb) {
    vector<int> shape {};
    dataDescript *dd = new dataDescript(name, shape, (_TREE *)inits);
    varManager->insert(dd, true);
}
void _DEF_ARR::translate(string ctn, string brk, bool glb) {
    vector<int> shape {};
    addr->vectorize(shape);
    dataDescript *dd = new dataDescript(name, shape, (_TREE *)inits);
    varManager->insert(dd);
    instantialize();
    if (!glb)
        initialize(glb);
    else
        globalInitList.push_back(this);
}
void _PARAM_ARR::translate(string ctn, string brk, bool glb) {
    vector<int> shape {0};
    if (addr) addr->vectorize(shape);
        /* addr = NULL in case name[] */
    dataDescript *dd = new dataDescript(name, shape, (_TREE *)inits);
    varManager->insert(dd, true);
}

/* ================================================= */
/* _FUNC_CALL::_FUNC_CALL                            */
/*      - support for stoptime & starttime           */
/* ================================================= */
_FUNC_CALL::_FUNC_CALL(string _name, sysyAST *_param): 
        name(_name), param(_param) {
    /* stoptime & starttime; */
    if (_name == "starttime") {
        name = "_sysy_starttime";
        param = (sysyAST *) new _CALL_LIST(new _INTEGER(yylineno), NULL);
    }
    if (_name == "stoptime") {
        name = "_sysy_stoptime";
        param = (sysyAST *) new _CALL_LIST(new _INTEGER(yylineno), NULL);
    }
}