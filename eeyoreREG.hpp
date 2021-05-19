#include <bits/stdc++.h>
#ifndef tiggerAST_hpp
#include "tiggerAST.hpp"
#define tiggerAST_hpp
#endif
using namespace std;

#define maxlines 1000
extern int currentLine ; 
extern void tiggerStmt(tiggerAST *x);
/* RegManager is in charge of allocation of registers,
and it is also responsible for var name record */
class Register {
    /* %tx %ax caller-save, %sx callee-save */
public:
    Register(string _name, int _id): reg_name(_name), reg_id(_id) {}
    /* static attribute */
    string reg_name; int reg_id;

    /* dynamic attribute           */
    /* active: allocated now       */
    /* used: ever allocated        */
    /* occupied: func-call used    */
    /* monopolized: used by global */
    bool occupied, monopolized, used;
    bitset<maxlines> active;

    void clear() { if (!monopolized) {active.reset(); used = false;} }
    bool compatible_global() {
        if (reg_name[0] == 'a') return false;
        /* %ax can't be allocated to global vars */
        if (used) return false;
        return monopolized = used = true;
    }
    bool compatible_local(bitset<maxlines> _active) {
        if (monopolized) return false;
        if ((active & _active).any()) return false;
        if (used) return false;
        active |= _active;
        return used = true;
    }
};
class Variable {
private:
    bool global, var, param;
    string eeyore_name;
    string tigger_name;
    int relative_addr;
public:
    Variable(bool _global, bool _var, bool _param, string _eeyore_name, string _tigger_name, int _relative_addr):
        global(_global), var(_var), param(_param), eeyore_name(_eeyore_name), tigger_name(_tigger_name), relative_addr(_relative_addr) {}
    bool isglobal() { return global; }
    bool isvar() { return var; }
    bool isparam() { return param; }
    string getTiggerName() { return tigger_name; }
    int getReaddr() { return relative_addr; }

    Register *alloc_reg;
    bitset<maxlines> active;
};

class RegManager {
public:
    map<string, Variable*> vars;

    int param_cnt, stack_size, global_cnt;

    string newGlobal(string s, bool isvar) {
        string tigger_name = "v" + to_string(global_cnt++);
        vars[s] = new Variable(true, isvar, false, s, tigger_name, -1);
        return tigger_name;
    }
    void newLocal(string s, int sz, bool isvar) {
        vars[s] = new Variable(false, isvar, s[0] == 'p', s, "local_var", stack_size);
        stack_size += sz;
    }
    bool isglobal(string s) { return vars[s]->isglobal(); }
    bool isvar(string s) { return vars[s]->isvar(); }
    int getReaddr(string s) { return vars[s]->getReaddr(); } /* returned value is the address with * 4 */
    string tigger(string s) { return vars[s]->getTiggerName(); }
    bool isparam(string s) { return vars[s]->isparam(); }
    Register *getAlloc(string s) { return vars.find(s) != vars.end() ? vars[s]->alloc_reg : NULL; }
    
    map<string, Register*> reg_ptr; //mapping: register_name -> register
    Register *registers[Reg_N];
    
    /* s11 & s10 & s9 & s8 are all reserved for global address & number respectively, can not be allocated */
    RegManager() {
        int register_cnt = 0;
        for (int i = 0; i < Reg_t; i++, register_cnt++)
            registers[register_cnt] = new Register("t" + to_string(i), register_cnt);
        for (int i = 0; i < Reg_a; i++, register_cnt++)
            registers[register_cnt] = new Register("a" + to_string(Reg_a - i - 1), register_cnt);
        for (int i = 0; i < Reg_s; i++, register_cnt++)
            registers[register_cnt] = new Register("s" + to_string(i), register_cnt);
        assert(register_cnt == Reg_N); 
        
        for (int i = 0; i < Reg_N; i++)
            reg_ptr[registers[i]->reg_name] = registers[i];
    }
    void store_reg(string reg_name, string var_name) {
        /* ============================= */
        /* address: do nothing           */
        /* var: assure it's not permanent*/
        /* ============================= */
        /* store register in heap / stack, depending on the type of variables */
        assert(vars.find(var_name) != vars.end());
        Variable *var = vars[var_name];
        Register *reg = var->alloc_reg;
        if (!var->isvar()) return ; /* no need to restore the start address of any array */
        assert(reg == NULL);
        if (var->isglobal()) {
            /* no STORE VAR REG in RISC-V */
            tiggerStmt(new _tLOADADDR(var->getTiggerName(), reserved_reg3));
            tiggerStmt(new _tSAVE(reserved_reg3, 0, reg_name));
        }
        else
            tiggerStmt(new _tSTORE(reg_name, var->getReaddr() >> 2));
    }
    void restore_reg(string var_name, string reg_name) {
        /* ============================= */
        /* all: assure it's not permanent*/
        /* ============================= */
        /* restore register from heap / stack, depending on the type of variables */
        assert(vars.find(var_name) != vars.end());
        Variable *var = vars[var_name];
        Register *reg = var->alloc_reg;
        assert(reg == NULL);
        if (var->isglobal()) {
            if (var->isvar())
                tiggerStmt(new _tLOAD(var->getTiggerName(), reg_name));
            else
                tiggerStmt(new _tLOADADDR(var->getTiggerName(), reg_name));
        }
        else {
            if (var->isvar())
                tiggerStmt(new _tLOAD(var->getReaddr() >> 2, reg_name));
            else
                tiggerStmt(new _tLOADADDR(var->getReaddr() >> 2, reg_name));
        }
    }
    void store(string reg_name, bool caller) {
        Register *reg = reg_ptr[reg_name];
        assert(reg != NULL);
        if (reg->used)
            tiggerStmt(new _tSTORE(reg_name, reg->reg_id));
        /*
        if (caller && reg->active[currentLine])
            tiggerStmt(new _tSTORE(reg_name, reg->reg_id));
        if (!caller && reg->used)
            tiggerStmt(new _tSTORE(reg_name, reg->reg_id));
            */
    }
    void restore(string reg_name, bool caller, Register *skip = NULL) {
        Register *reg = reg_ptr[reg_name];
        assert(reg != NULL);

        reg->occupied = false;
        /* if previously being occupied by param,
            now it is recovered */
        if (reg == skip) return ;
        if (reg->used)
            tiggerStmt(new _tLOAD(reg->reg_id, reg_name));
        /*
        if (caller && reg->active[currentLine])
            tiggerStmt(new _tLOAD(reg->reg_id, reg_name));
        if (!caller && reg->used)
            tiggerStmt(new _tLOAD(reg->reg_id, reg_name));*/
    }
    void caller_store() {
        /* in charge of storation of registers %tx %ax */
        for (int i = 0; i < Reg_t; i++)
            store("t" + to_string(i), true);
        for (int i = param_cnt; i < Reg_a; i++)
        /* because of the structure of eeyore Program,
            Param appears before function call,
            so some register storation is done before */
            store("a" + to_string(i), true);
    }
    void caller_restore(Register *skip = NULL) {
        /* in charge of restoration of registers %tx %ax */
        for (int i = 0; i < Reg_t; i++)
            restore("t" + to_string(i), true, skip);
        for (int i = 0; i < Reg_a; i++)
            restore("a" + to_string(i), true, skip);
        /* warning: reset register's available flag */
    }
    void callee_store() {
        /* in charge of storation of registers %sx */
        for (int i = 0; i < Reg_s; i++)
            store("s" + to_string(i), false);
    }
    void callee_restore() {
        /* in charge of restoration of registers %sx */
        for (int i = 0; i < Reg_s; i++)
            restore("s" + to_string(i), false);
    }
    void new_environ() {
        for (int i = 0; i < Reg_N; i++)
            registers[i]->clear();
        /* warning: alloc_reg.clean() is not called*/
        /* bottom of the stack is reserved for callee-registers */
        stack_size = Reg_N << 2;
    }
    void preload(string var_name) {
        Variable *var = vars[var_name];
        Register *reg = var->alloc_reg;
        if (reg == NULL) return ; /* this var is not gonna be stored in register */
        if (var->isvar()) return ; /* no need to read address for var */
        if (var->isglobal())
            tiggerStmt(new _tLOADADDR(var->getTiggerName(), reg->reg_name));
        else
            tiggerStmt(new _tLOADADDR(var->getReaddr() >> 2, reg->reg_name));
    }
    void must_allocate(string var_name, string reg_name) {
        Register *reg = reg_ptr[reg_name];
        Variable *var = vars[var_name];
        assert(reg->compatible_local(var->active));
        var->alloc_reg = reg;
    }
    void try_allocate(string var_name) {
        Variable *var = vars[var_name];
        var->alloc_reg = NULL;
        return ;
        if (var->isglobal()) {
            for (int i = 0; i < Reg_N; i++)
                if (registers[i]->compatible_global())
                    {var->alloc_reg = registers[i]; cerr << var_name << " was allocated to " << registers[i]->reg_name << endl; return ; }
        }
        else {
            for (int i = 0; i < Reg_N; i++)
                if (registers[i]->compatible_local(var->active))
                    {var->alloc_reg = registers[i]; cerr << var_name << " was allocated to " << registers[i]->reg_name << endl; return ; }
        }
    }
};