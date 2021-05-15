#ifndef defs_hpp
#include "defs.hpp"
#define defs_hpp
#endif
using namespace std;

/* auxiliary module */
string load_into_register(eeyoreAST *var, string default_reg) {
    /* If the given variable is already in registers, return the register name.
    Otherwise, load the var into the default register*/
    Register *var_reg = regManager->alloc_reg[var->getName()];
    if (var_reg != NULL) return var_reg->reg_name;

    if (var->isnum())
        tiggerStmt(new _tDIRECT(default_reg, var->getInt()));
    else
        regManager->restore_reg(var->getName(), default_reg);
    return default_reg;
}
string load_into_register_nonum(string var, string default_reg) {
    /* If the given variable is already in registers, return the register name.
    Otherwise, load the var into the default register*/
    Register *var_reg = regManager->alloc_reg[var];
    if (var_reg != NULL) return var_reg->reg_name;

    regManager->restore_reg(var, default_reg);
    return default_reg;
}
/* essentially, this cpp file is the implementation of eeyore AST's translation. */

void _eDEFVAR::translate() {
    /* only global vars could enter this function */
    string _var = regManager->setglobal(var, true);
    tiggerDecl(new _tGLBVAR(_var));
}
void _eDEFVAR::try_allocate() {
    regManager->setlocal(var, 4, true);
    regManager->try_allocate(var);
}
void _eDEFARR::translate() {
    /* only global vars could enter this function */
    string _var = regManager->setglobal(var, false);
    tiggerDecl(new _tGLBARR(_var, size));
}
void _eDEFARR::try_allocate() {
    regManager->setlocal(var, size << 2, false);
    // regManager->try_allocate(var);
    /* potential optimization here: local array's start address could be stored in register */
}
void _eDIRECT::translate() {
    /* a = t, a:var, t:var,int*/
    Register *a_reg = regManager->alloc_reg[a->getName()];
    Register *t_reg = regManager->alloc_reg[t->getName()];
    if (a_reg && t_reg)
        tiggerStmt(new _tDIRECT(a_reg->reg_name, t_reg->reg_name));
    else if (a_reg && !t_reg) {
        if (t->isnum())
            tiggerStmt(new _tDIRECT(a_reg->reg_name, t->getInt()));
        else
            regManager->restore_reg(t->getName(), a_reg->reg_name);
    }
    else if (!a_reg && t_reg)
        regManager->store_reg(t_reg->reg_name, a->getName());
    else if (!a_reg && !t_reg) {
        if (t->isnum())
            tiggerStmt(new _tDIRECT(reserved_reg1, t->getInt()));
        else
            regManager->restore_reg(t->getName(), reserved_reg1);
        regManager->store_reg(reserved_reg1, a->getName());
    }
}
void _eUNARY::translate() {
    /* a = op t, a:var, t:var,int*/
    Register *a_reg = regManager->alloc_reg[a->getName()];
    Register *t_reg = regManager->alloc_reg[t->getName()];
    if (a_reg && t_reg)
        tiggerStmt(new _tUNARY(a_reg->reg_name, op, t_reg->reg_name));
    else if (a_reg && !t_reg) {
        if (t->isnum())
            tiggerStmt(new _tDIRECT(a_reg->reg_name, unary_result(op, t->getInt())));
        else {
            regManager->restore_reg(t->getName(), reserved_reg1);
            tiggerStmt(new _tUNARY(a_reg->reg_name, op, reserved_reg1));
        }
    }
    else if (!a_reg) {
        if (t_reg)
            tiggerStmt(new _tUNARY(reserved_reg1, op, t_reg->reg_name));
        else if (t->isnum())
            tiggerStmt(new _tDIRECT(reserved_reg1, unary_result(op, t->getInt())));
        else {
            regManager->restore_reg(t->getName(), reserved_reg2);
            tiggerStmt(new _tUNARY(reserved_reg1, op, reserved_reg2));
        }
        regManager->store_reg(reserved_reg1, a->getName());
    }
}
void _eBINARY::translate() {
    /* a = t1 op t2, a:var, t1:var,int, t2:var,int */
    Register *a_reg = regManager->alloc_reg[a->getName()];
    Register *t1_reg = regManager->alloc_reg[t1->getName()];
    Register *t2_reg = regManager->alloc_reg[t2->getName()];
    if (t1->isnum() && t2->isnum()) {
        if (a_reg)
            tiggerStmt(new _tDIRECT(a_reg->reg_name, binary_result(t1->getInt(), op, t2->getInt())));
        else {
            tiggerStmt(new _tDIRECT(reserved_reg1, binary_result(t1->getInt(), op, t2->getInt())));
            regManager->store_reg(reserved_reg1, a->getName());
        }
    }
    else {
        string t1_reg_name = load_into_register(t1, reserved_reg2);
        string t2_reg_name = load_into_register(t2, reserved_reg3);
        /* potential optimization here: t2 may be an integer */
        if (a_reg)
            tiggerStmt(new _tBINARY(a_reg->reg_name, t1_reg_name, op, t2_reg_name));
        else {
            tiggerStmt(new _tBINARY(reserved_reg1, t1_reg_name, op, t2_reg_name));
            regManager->store_reg(reserved_reg1, a->getName());
        }
    }
}
void _eSEEK::translate() {
    /* a = t[x], a:var, t:address, x:var,int */
    Register *a_reg = regManager->alloc_reg[a->getName()];
    Register *t_reg = regManager->alloc_reg[t];
    Register *x_reg = regManager->alloc_reg[x->getName()];
    if (regManager->isglobal(t)) {
        string t_reg_name = load_into_register_nonum(t, reserved_reg1);
        string x_reg_name = load_into_register(x, reserved_reg2);
        /* potential optimization here: x is an integer */
        
        tiggerStmt(new _tBINARY(reserved_reg1, t_reg_name, "+", x_reg_name));
        /* warning: value stored in reserved_reg1 is only the address */
        if (a_reg)
            tiggerStmt(new _tSEEK(a_reg->reg_name, reserved_reg1, 0));
        else {
            
            tiggerStmt(new _tSEEK(reserved_reg2, reserved_reg1, 0));
            regManager->store_reg(reserved_reg2, a->getName());
        }

    }
    else {
        int t_addr = regManager->getreaddr(t);
        if (x->isnum()) {
            if (a_reg)
                tiggerStmt(new _tLOAD((t_addr >> 2) + x->getInt(), a_reg->reg_name));
            else {
                tiggerStmt(new _tLOAD((t_addr >> 2) + x->getInt(), reserved_reg1));
                regManager->store_reg(reserved_reg1, a->getName());
            }
        }
        else {
            string t_reg_name = load_into_register_nonum(t, reserved_reg1);
            string x_reg_name = load_into_register(x, reserved_reg2);
            tiggerStmt(new _tBINARY(reserved_reg1, t_reg_name, "+", x_reg_name));
            /* warning: value stored in reserved_reg1 is only the address */
            if (a_reg)
                tiggerStmt(new _tSEEK(a_reg->reg_name, reserved_reg1, 0));
            else {
                tiggerStmt(new _tSEEK(reserved_reg2, reserved_reg1, 0));
                regManager->store_reg(reserved_reg2, a->getName());
            }
        }
    }
}
void _eSAVE::translate() {
    /* a[x] = t, a:addr, t:addr,int,var, x:var,int */
    Register *a_reg = regManager->alloc_reg[a];
    Register *t_reg = regManager->alloc_reg[t->getName()];
    Register *x_reg = regManager->alloc_reg[x->getName()];
    if (regManager->isglobal(a)) {
        string a_reg_name = load_into_register_nonum(a, reserved_reg1);
        /* potential optimization here: t is an integer */
        if (x->isnum()) {
            string t_reg_name = load_into_register(t, reserved_reg2);
            tiggerStmt(new _tSAVE(a_reg_name, x->getInt(), t_reg_name));
        }
        else {
            string x_reg_name = load_into_register(x, reserved_reg2);
            tiggerStmt(new _tBINARY(reserved_reg1, a_reg_name, "+", x_reg_name));
            string t_reg_name = load_into_register(t, reserved_reg2);
            tiggerStmt(new _tSAVE(reserved_reg1, 0, t_reg_name));
        }
    }
    else {
        int a_addr = regManager->getreaddr(a);
        if (x->isnum()) {
            string t_reg_name = load_into_register(t, reserved_reg1);
            tiggerStmt(new _tSTORE(t_reg_name, (a_addr >> 2) + x->getInt()));
        }
        else {
            string a_reg_name = load_into_register_nonum(a, reserved_reg1);
            string x_reg_name = load_into_register(x, reserved_reg2);
            /* potential optimization here: a's address may be calculated */
            tiggerStmt(new _tBINARY(reserved_reg1, a_reg_name, "+", x_reg_name));
            string t_reg_name = load_into_register(t, reserved_reg2);
            tiggerStmt(new _tSAVE(reserved_reg1, 0, t_reg_name));
        }
    }
}
void _eFUNCRET::translate() {
    regManager->caller_store();
    /* before call */

    /* a = call f_func */
    tiggerStmt(new _tCALL(func));
    Register *a_reg = regManager->alloc_reg[a->getName()];
    if (a_reg) {
        tiggerStmt(new _tDIRECT(a_reg->reg_name, "a0"));
        /* skip restore a_reg, this step is necessray.
            otherwise, value before update will overwrite the right value. */
        regManager->store_reg(a_reg->reg_name, a->getName());
        /* warning: if global var is allowed to be stored in register,
            write-back may be needed in this scenario */
    }
    else
        regManager->store_reg("a0", a->getName());
    
    /* after call */
    regManager->caller_restore(a_reg);
    regManager->param_cnt = 0;
    /* param_cnt must be reset after caller_store */
}
void _eCALL::translate() {
    regManager->caller_store();
    /* before call */

    tiggerStmt(new _tCALL(func));

    /* after call */
    regManager->caller_restore();
    regManager->param_cnt = 0;
}
void _ePARAM::translate() {
    Register *t_reg = regManager->alloc_reg[t->getName()];
    string reg_name = "a" + to_string(regManager->param_cnt++);
    regManager->store(reg_name);
    /* potential optimization here: param is luckily just in %ai */
    if (t_reg)
        tiggerStmt(new _tDIRECT(reg_name, t_reg->reg_name));
    else if (t->isnum())
        tiggerStmt(new _tDIRECT(reg_name, t->getInt()));
    else
        regManager->restore_reg(t->getName(), reg_name);
}
void _eIFGOTO::translate() {
    assert(t2->getName() == "0" && (op == "!=" || op == "=="));
    /* in the transfomation of sysY->eeyore, t2 is guaranteed as 0 */
    if (t1->isnum()) {
        /* if the condition can not be true, this jump instruction is useless */
        if ((op == "==") == (t1->getName() == t2->getName()))
            tiggerStmt(new _tGOTO(l));
    }
    else {
        string t1_reg_name = load_into_register_nonum(t1->getName(), reserved_reg1);
        tiggerStmt(new _tIFGOTO(t1_reg_name, op, "x0", l));
    }
}
void _eGOTO::translate() {
    tiggerStmt(new _tGOTO(l));
}
void _eLABEL::translate() {
    tiggerStmt(new _tLABEL(l));
}
void _eSEQ::translate() {
    /* every function's body(_eSEQ) can't enter this function */
    for (auto stmt: seq)
        stmt->translate();
    tiggerRoot = new _tSEQ(tiggerList);
}
void _eFUNC::translate() {
    _tFUNC *tfunc = new _tFUNC(func, arity);
    regManager->new_environ();
    /* param is always in register */
    for (int i = 0; i < arity; i++) {
        regManager->setlocal("p" + to_string(i), 4, true);
        regManager->must_allocate("p" + to_string(i), "a" + to_string(i));
    }
    auto seq = ((_eSEQ *) body)->seq;
    for (auto stmt: seq)
        if (stmt->isdef()) stmt->try_allocate();
    tfunc->mem = regManager->stack_size >> 2;
    regManager->callee_store();
    for (auto stmt: seq)
        if (!stmt->isdef()) stmt->translate();
    
    /* function structure:
        func [arity] [mem] body */
    tfunc->body = new _tSEQ(tiggerStmtList);
    tiggerStmtList.clear();
    tiggerList.push_back(tfunc);
}
void _eRET::translate() {
    Register *t_reg = regManager->alloc_reg[t->getName()];
    regManager->callee_restore();
    regManager->restore("a0");
    if (t_reg)
        tiggerStmt(new _tDIRECT("a0", t_reg->reg_name));
    else if (t->isnum())
        tiggerStmt(new _tDIRECT("a0", t->getInt()));
    else
        regManager->restore_reg(t->getName(), "a0");
    tiggerStmt(new _tRETURN());
}
void _eRETVOID::translate() {
    regManager->callee_restore();
    tiggerStmt(new _tRETURN());
}