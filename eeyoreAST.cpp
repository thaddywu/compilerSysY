#ifndef defs_hpp
#include "defs.hpp"
#define defs_hpp
#endif
using namespace std;

/* auxiliary module */
string load_into_register(eeyoreAST *var, string default_reg) {
    /* If the given variable is already in registers, return the register name.
    Otherwise, load the var into the default register*/
    Register *var_reg = regManager->getAlloc(var->getName());
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
    Register *var_reg = regManager->getAlloc(var);
    if (var_reg != NULL) return var_reg->reg_name;

    regManager->restore_reg(var, default_reg);
    return default_reg;
}
/* essentially, this cpp file is the implementation of eeyore AST's translation. */

/* ================================================= */
/* decl                                              */
/*      - declare variables & arrays                 */
/*      - reg allocation implemented in ::optimize   */
/* ================================================= */
void _eDEFVAR::globalDecl() {
    /* only global vars could enter this function */
    string _var = regManager->newGlobal(var, true);
    tiggerDecl(new _tGLBVAR(_var));
}
void _eDEFARR::globalDecl() {
    /* only global vars could enter this function */
    string _var = regManager->newGlobal(var, false);
    tiggerDecl(new _tGLBARR(_var, size));
}
void _eDEFVAR::localDecl() {
    /* only local vars could enter this function */
    regManager->newLocal(var, 4, true);
}
void _eDEFARR::localDecl() {
    /* only local vars could enter this function */
    regManager->newLocal(var, size << 2, false);
}

/* ================================================= */
/* translate                                         */
/*      - translate to tigger AST                    */
/*      - optimized version for _eSEQ & _eFUNC       */
/* ================================================= */
void _eDIRECT::translate() {
    /* a = t, a:var, t:var,int*/
    Register *a_reg = regManager->getAlloc(a->getName());
    Register *t_reg = regManager->getAlloc(t->getName());
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
    Register *a_reg = regManager->getAlloc(a->getName());
    Register *t_reg = regManager->getAlloc(t->getName());
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
    Register *a_reg = regManager->getAlloc(a->getName());
    Register *t1_reg = regManager->getAlloc(t1->getName());
    Register *t2_reg = regManager->getAlloc(t2->getName());
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
        /* potential optimization here: t2 may be an integer(int12) */
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
    Register *a_reg = regManager->getAlloc(a->getName());
    Register *t_reg = regManager->getAlloc(t);
    Register *x_reg = regManager->getAlloc(x->getName());
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
        int t_addr = regManager->getReaddr(t);
        /* warning: t could be the param passed by outside function */
        if (x->isnum() && !regManager->isparam(t)) {
            if (a_reg)
                tiggerStmt(new _tLOAD((t_addr + x->getInt()) >> 2 , a_reg->reg_name));
            else {
                tiggerStmt(new _tLOAD((t_addr + x->getInt()) >> 2, reserved_reg1));
                regManager->store_reg(reserved_reg1, a->getName());
            }
        }
        else {
            string x_reg_name = load_into_register(x, reserved_reg1);
            /* potential optimization here: x may be an integer */
            string t_reg_name ;
            if (regManager->isparam(t)) {
                assert(t_reg != NULL); /* t must be stored in register */
                t_reg_name = t_reg->reg_name;
            }
            else {
                tiggerStmt(new _tLOADADDR(t_addr >> 2, reserved_reg2));
                t_reg_name = reserved_reg2;
            }
            
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
    Register *a_reg = regManager->getAlloc(a);
    Register *t_reg = regManager->getAlloc(t->getName());
    Register *x_reg = regManager->getAlloc(x->getName());
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
        int a_addr = regManager->getReaddr(a);
        if (x->isnum() && !regManager->isparam(a)) {
            string t_reg_name = load_into_register(t, reserved_reg1);
            tiggerStmt(new _tSTORE(t_reg_name, (a_addr + x->getInt()) >> 2));
        }
        else {
            string x_reg_name = load_into_register(x, reserved_reg1);
            /* potential optimization here: x may be an integer, or a's address may be calculated */
            string a_reg_name ;
            if (regManager->isparam(a)) {
                assert(a_reg != NULL); /* a must be stored in register */
                a_reg_name = a_reg->reg_name;
            }
            else {
                tiggerStmt(new _tLOADADDR(a_addr >> 2, reserved_reg2));
                a_reg_name = reserved_reg2;
            }
            
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
    Register *a_reg = regManager->getAlloc(a->getName());
    if (a_reg) {
        tiggerStmt(new _tDIRECT(a_reg->reg_name, "a0"));
        /* skip restore a_reg, this step is necessray.
            otherwise, value before update will overwrite the right value. */
        // regManager->store_reg(a_reg->reg_name, a->getName()); 
        /* warning: if global var is allowed to be stored in register,
            write-back may be needed in this scenario */
    }
    else
        regManager->store_reg("a0", a->getName());
    
    /* after call */
    regManager->caller_restore(a_reg);
    regManager->param_cnt = 0;
    /* param_cnt must be reset after caller_store ??? */
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
    Register *t_reg = regManager->getAlloc(t->getName());
    string reg_name = "a" + to_string(regManager->param_cnt++);
    
        cerr << "param.." << reg_name << " " << t->getName() << endl;
    regManager->store(reg_name, true);
    /* potential optimization here: param is luckily just in %ai */
    if (t_reg) /* warning */ {
        cerr << " .." << t_reg->reg_name << " " << t_reg->occupied << endl;
        if (t_reg->occupied)
            tiggerStmt(new _tLOAD(t_reg->reg_id, reg_name));
        else
            tiggerStmt(new _tDIRECT(reg_name, t_reg->reg_name));
        /* here %ax is already replaced by the parameter, its real value is in stack */
    }
    else if (t->isnum())
        tiggerStmt(new _tDIRECT(reg_name, t->getInt()));
    else
        regManager->restore_reg(t->getName(), reg_name);

    regManager->reg_ptr[reg_name]->occupied = true;
    /* if a0 is overwritten,
        value needs to be load back from stack */
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
void _eRET::translate() {
    Register *t_reg = regManager->getAlloc(t->getName());
    if (t_reg)
        tiggerStmt(new _tDIRECT("a0", t_reg->reg_name));
    else if (t->isnum())
        tiggerStmt(new _tDIRECT("a0", t->getInt()));
    else
        regManager->restore_reg(t->getName(), "a0");
        /* pass returned value, then callee_save. a0 = t1. */
    regManager->callee_restore();
    tiggerStmt(new _tRETURN());
}
void _eRETVOID::translate() {
    regManager->callee_restore();
    tiggerStmt(new _tRETURN());
}
void _eSEQ::translate() { optimize(); }
void _eFUNC::translate() { optimize(); }