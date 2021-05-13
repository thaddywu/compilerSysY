#ifndef defs_hpp
#include "defs.hpp"
#define defs_hpp
#endif
using namespace std;

/* essentially, this cpp file is the implementation of eeyore AST's translation. */

void _eDEFVAR::translate(bool glb, int &mem) {
    if (glb) {
        tiggerDecl(new _tGLBVAR(var));
        regManager->setglobal(var);
    }
    else {
        mem += 1;
        regManager->setlocal(var, 4);
    }
}
void _eDEFARR::translate(bool glb, int &mem) {
    if (glb) {
        tiggerDecl(new _tGLBARR(var, size));
        regManager->setglobal(var);
    }
    else {
        mem += size;
        regManager->setlocal(var, size << 2);
    }
}
void _eDIRECT::translate(bool glb, int &mem) {
    string t_reg = regManager->getRegister(t->getName(), true);
    string a_reg = regManager->getRegister(a->getName(), false);
    tiggerStmt(new _tDIRECT(a_reg, t_reg));
    regManager->dirty(a_reg);
}
void _eUNARY::translate(bool glb, int &mem) {
    if (t->isnum()) {
        string a_reg = regManager->getRegister(a->getName(), false);
        tiggerStmt(new _tDIRECT(a_reg, unary_result(op, t->getInt())));
        regManager->dirty(a_reg);
    }
    else {
        string t_reg = regManager->getRegister(t->getName(), true);
        string a_reg = regManager->getRegister(a->getName(), false);
        tiggerStmt(new _tUNARY(a_reg, op, t_reg));
        regManager->dirty(a_reg);
    }
}

void _eBINARY::translate(bool glb, int &mem) {
    if (t1->isnum() && t2->isnum()) {
        string a_reg = regManager->getRegister(a->getName(), false);
        tiggerStmt(new _tDIRECT(a_reg, binary_result(t1->getInt(), op, t2->getInt())));
        regManager->dirty(a_reg);
    }
    else {
        if (t1->isnum()) swap(t1, t2);
        string t1_reg = regManager->getRegister(t1->getName(), true);
        string t2_reg = regManager->getRegister(t2->getName(), true);
        string a_reg = regManager->getRegister(a->getName(), false);
        tiggerStmt(new _tBINARY(a_reg, t1_reg, op, t2_reg));
        regManager->dirty(a_reg);
    }
}
void _eSEEK::translate(bool glb, int &mem) { // a = t[x]
    if (regManager->isglobal(t)) {
        string t_reg = regManager->getRegister(t, true);
        string a_reg = regManager->getRegister(a->getName(), false);
        if (x->isnum())
            tiggerStmt(new _tSEEK(a_reg, t_reg, x->getInt()));
        else {
            tiggerStmt(new _tBINARY("s11", t_reg, "+", x->getName()));
            tiggerStmt(new _tSEEK(a_reg, "s11", 0));
        }
        regManager->dirty(a_reg);
    }
    else {
        if (x->isnum()) {
            string a_reg = regManager->getRegister(a->getName(), false);
            tiggerStmt(new _tLOAD(regManager->getreaddr(t) + x->getInt(), a_reg));
            regManager->dirty(a_reg);
        }
        else {
            string x_reg = regManager->getRegister(x->getName(), true);
            string a_reg = regManager->getRegister(a->getName(), false);
            tiggerStmt(new _tLOADADDR("s11", t));
            tiggerStmt(new _tBINARY("s11", "s11", "+", x_reg));
            tiggerStmt(new _tSEEK(a_reg, "s11", 0));
            regManager->dirty(a_reg);
        }
    }
}
void _eSAVE::translate(bool glb, int &mem) { // a[x] = t
    string t_reg = t->isnum() ? "s10" : regManager->getRegister(t->getName(), true);
    if (t->isnum())
        tiggerStmt(new _tDIRECT("s10", t->getInt()));
    /* integer should not be dumped into s11, because
        s11 may be used in some other instructions. */

    if (regManager->isglobal(a)) {
        string a_reg = regManager->getRegister(a, true);
        if (x->isnum()) {
            tiggerStmt(new _tSAVE(a_reg, x->getInt(), t_reg));
        }
        else {
            tiggerStmt(new _tBINARY("s11", a_reg, "+", x->getName()));
            tiggerStmt(new _tSAVE("s11", 0, t_reg));
        }
    }
    else {
        if (x->isnum())
            tiggerStmt(new _tSTORE(t_reg, regManager->getreaddr(a) + x->getInt()));
        else {
            string x_reg = regManager->getRegister(x->getName(), true);
            tiggerStmt(new _tLOADADDR("s11", a));
            tiggerStmt(new _tBINARY("s11", "s11", "+", x_reg));
            tiggerStmt(new _tSAVE("s11", 0, t_reg));
        }
    }
}
void _eFUNCRET::translate(bool glb, int &mem) {
    tiggerStmt(new _tRETURN());
    regManager->assign("a0", a->getName());
}
void _eCALL::translate(bool glb, int &mem) {
    tiggerStmt(new _tRETURN());
}
void _ePARAM::translate(bool glb, int &mem) {
    string reg = "a" + regManager->newParam();
    if (regManager->)
}
void _eIFGOTO::translate(bool glb, int &mem) {
    assert(t2->getName() == "0" && (op == "!=" || op == "=="));
    /* in the transfomation of sysY->eeyore, t2 is guaranteed as 0 */
    if (t1->isnum()) {
        if ((op == "==") == (t1->getName() == t2->getName()))
            tiggerStmt(new _tGOTO(l));
        /* if the condition can not be true, this jump instruction is useless */
    }
    else
        tiggerStmt(new _tIFGOTO(t1->getName(), op, "x0", l));
}
void _eGOTO::translate(bool glb, int &mem) {
    tiggerStmt(new _tGOTO(l));
}
void _eLABEL::translate(bool glb, int &mem) {
    tiggerStmt(new _tLABEL(l));
}