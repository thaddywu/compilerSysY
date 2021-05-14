#include <bits/stdc++.h>
#ifndef tiggerAST_hpp
#include "tiggerAST.hpp"
#define tiggerAST_hpp
#endif
using namespace std;

#define Reg_s 9
#define Reg_t 7
#define Reg_a 8
#define Reg_N (Reg_s + Reg_t + Reg_a)

#define reserved_reg1 "s9"
#define reserved_reg2 "s10"
#define reserved_reg3 "s11"

extern void tiggerStmt(tiggerAST *x);
/* RegManager is in charge of allocation of registers,
and it is also responsible for var name record */
class Register {
public:
    Register(string _name): reg_name(_name) {}
    
    string reg_name;
    bool allocated;
    string allocated_var;

    bool contain(string s) {
        return allocated && allocated_var == s;
    }
    void new_environ() { allocated = false; }
};

class RegManager {
public:
    map<string, bool> _global; //mapping: global var/array -> if is var
    map<string, bool> _isvar;
    map<string, int> readdr; //mapping: local var/array -> relative address on stack (with * 4)

    int next_vacant_reg, param_cnt, stack_size, global_cnt;

    void setglobal(string s, bool isvar) { _global[s] = true; _isvar[s] = isvar; }
    bool isglobal(string s) { return _global.find(s) != _global.end() ? _global[s] : false; }
    void setlocal(string s, int sz, bool isvar) { readdr[s] = sz; _isvar[s] = isvar; stack_size += sz; }
    bool isvar(string s) { return _isvar[s]; }
    int getreaddr(string s) { assert(!isglobal(s)); return readdr[s] >> 2; } /* returned value is the index without *4 */
    
    map<string, Register*> alloc_reg;
    map<string, Register*> reg_ptr; //mapping: register_name -> register
    Register *registers[Reg_N];
    
    // bool isalternative(string s) { return reg_ptr.find(s) == reg_ptr.end(); }
    /* s11 & s10 is reserved for global address & number respectively, can not be allocated */
    RegManager() {
        for (int i = 0; i < Reg_t; i++)
            registers[i] = new Register("t" + to_string(i));
        for (int i = 0; i < Reg_s; i++)
            registers[i + Reg_t] = new Register("s" + to_string(i));
        for (int i = 0; i < Reg_a; i++)
            registers[i + Reg_t + Reg_s] = new Register("a" + to_string(Reg_a - i - 1));
        
        for (int i = 0; i < Reg_N; i++)
            reg_ptr[registers[i]->reg_name] = registers[i];
    }
    void store_reg(string reg, string var) {
        /* store register in heap / stack, depending on the type of variables */
        if (!isvar(var)) return ; /* no need to restore the start address of any array */
        if (isglobal(var)) {
            /* no STORE VAR REG in RISC-V */
            tiggerStmt(new _tLOADADDR(var, reserved_reg3));
            tiggerStmt(new _tSAVE(reserved_reg3, 0, reg));
        }
        else
            tiggerStmt(new _tSTORE(reg, getreaddr(var)));
    }
    void restore_reg(string var, string reg) {
        /* restore register from heap / stack, depending on the type of variables */
        if (isglobal(var)) {
            if (isvar(var))
                tiggerStmt(new _tLOAD(var, reg));
            else
                tiggerStmt(new _tLOADADDR(var, reg));
        }
        else {
            if (isvar(var))
                tiggerStmt(new _tLOAD(getreaddr(var), reg));
            else
                tiggerStmt(new _tLOADADDR(getreaddr(var), reg));
        }
    }
    void store(string reg_name) {
        Register *reg = reg_ptr[reg_name];
        if (reg != NULL && reg->allocated)
            store_reg(reg_name, reg->allocated_var);
    }
    void restore(string reg_name) {
        Register *reg = reg_ptr[reg_name];
        if (reg != NULL && reg->allocated)
            restore_reg(reg->allocated_var, reg_name);
    }
    void caller_store() {
        /* in charge of storation of registers %tx %ax */
        for (int i = 0; i < Reg_t; i++)
            store("t" + to_string(i));
        for (int i = param_cnt; i < Reg_a; i++)
            store("a" + to_string(i));
    }
    void caller_restore() {
        /* in charge of restoration of registers %tx %ax */
        for (int i = 0; i < Reg_t; i++)
            restore("t" + to_string(i));
        for (int i = param_cnt; i < Reg_a; i++)
            restore("a" + to_string(i));
    }
    void callee_store() {
        /* in charge of storation of registers %sx */
        for (int i = 0; i < Reg_s; i++)
            tiggerStmt(new _tSTORE("s" + to_string(i), i));
    }
    void callee_restore() {
        /* in charge of restoration of registers %sx */
        for (int i = 0; i < Reg_s; i++)
            tiggerStmt(new _tLOAD(i, "s" + to_string(i)));
    }
    void new_environ() {
        next_vacant_reg = 0;
        for (int i = 0; i < Reg_N; i++)
            registers[i]->new_environ();
        /* bottom of the stack is reserved for callee-registers */
        stack_size = Reg_s << 2;
    }
    void try_allocate(string var) {
        if (next_vacant_reg >= Reg_N) return ;
        Register *reg = registers[next_vacant_reg++];
        reg->allocated = true;
        reg->allocated_var = var;
    }
};