#include <bits/stdc++.h>
#ifndef tiggerAST_hpp
#include "tiggerAST.hpp"
#define tiggerAST_hpp
#endif
using namespace std;

#define Reg_s 10
#define Reg_t 7
#define Reg_a 8
#define Reg_N (Reg_s + Reg_t + Reg_a)

extern void tiggerStmt(tiggerAST *x);
/* RegManager is in charge of allocation of registers,
and it is also responsible for var name record */
class RegManager {
public:
    map<string, bool> _global; //mapping: global var/array -> if is var
    map<string, bool> _isvar;
    map<string, int> readdr; //mapping: local var/array -> relative address on stack (with * 4)

    void setglobal(string s, bool isvar) { _global[s] = true; _isvar[s] = isvar; }
    bool isglobal(string s) { return _global.find(s) != _global.end() ? _global[s] : false; } /* no multiple variables with a same name */
    void setlocal(string s, int addr, bool isvar) { readdr[s] = addr; _isvar[s] = isvar; }
    bool isvar(string s) { return _isvar[s]; }
    int getreaddr(string s) { assert(!isglobal(s)); return readdr[s]; }

    int timeStamp;
    struct Register {
        Register(string name): allocated(false), lastAccess(0), reg_name(name), ever_used(false) {}
        bool allocated;
        int lastAccess;
        string allocated_var;
        string reg_name;
        bool dirty;
        bool ever_used; // has it been 
        bool write_back; // write back when released?
        /* enter into a new function, previous info should be cleared */
        bool contain(string s) { return allocated && allocated_var == s; }
        void store() { tiggerStmt(new _tSTORE(reg_name, getreaddr(reg_name))); }
        void restore() { tiggerStmt(new _tLOAD(getreaddr(reg_name), reg_name)); }
        void refresh() {allocated = dirty = ever_used = false; lastAccess = 0;}
    };
    map<string, Register*> reg_ptr; //mapping: register_name -> register
    Register *registers[Reg_N];
    int existed_param = 0;
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

    void dirty(string s) { assert (reg_ptr.find(s) != reg_ptr.end()); reg_ptr[s]->dirty = true; }
    bool isallocated(string s) {
        for (int i = 0; i < Reg_N; i++)
            if (registers[i]->contain(s)) return true;
        return false;
    }
    void writeback(Register *reg) {
        assert(reg->allocated);
        if (!reg->dirty) return ;
        if (!reg->write_back) return ;
        /* if register is not modified, or explicitly set,
            no write-back is needed */
        reg->dirty = false;
        if (isglobal(reg->allocated_var)) {
            /* global array's address should not be modified anytime.
            Don't need to care that conditio,
            because it must be filtered by !reg->dirty */
            tiggerStmt(new _tLOADADDR(reg->allocated_var, "s11"));
                /* possible redundance here */
            tiggerStmt(new _tSAVE("s11", 0, reg->reg_name));
        }
        else {
            assert(readdr.find(reg->allocated_var) != readdr.end());
            tiggerStmt(new _tSTORE(reg->reg_name, readdr[reg->allocated_var]));
        }
    }
    void writeback(string reg_name) {
        writeback(reg_ptr[reg_name]);
    }
    void release(Register *reg) {
        /* release register, and write back */
        if (!reg->allocated) return ;
        /* potential optimization here:
            if corresponding variable is no longer useful,
            there's no need to write back*/
        reg->allocated = false;
        writeback(reg);
    }
    void release(string reg_name) {
        release(reg_ptr[reg_name]);
    }
    void assign(Register *reg, string s, bool write_back) {
        /* allocate register(reg_name) to s,
            real allocation is done somewhere else */
        reg->allocated = true;
        reg->allocated_var = s;
        reg->lastAccess = ++timeStamp;
        reg->ever_used = true;
        reg->dirty = true;
        reg->write_back = write_back;
    }
    void assign(string reg_name, string s, bool write_back) {
        assign(reg_ptr[reg_name], s, write_back);
    }
    void load(Register *reg, string s) {
        /* load value from stack or memory into register */
        if (!reg->allocated) return ;
        assert(reg->contain(s));
        if (isglobal(s)) {
            if (isvar(s))
                tiggerStmt(new _tLOAD(s, reg->reg_name));
            else
                tiggerStmt(new _tLOADADDR(s, reg->reg_name));
            reg->dirty = false;
        }
        else {
            if (isvar(s))
                tiggerStmt(new _tLOAD(getreaddr(s), reg->reg_name));
            else
                tiggerStmt(new _tLOADADDR(getreaddr(s), reg->reg_name));
            reg->dirty = false;
        }
    }
    string getRegister(string s, bool reload) {
        Register *ret = NULL;
        for (int i = 0; i < Reg_N; i++)
        if (!registers[i]->allocated) {
            if (ret == NULL)
                ret = registers[i];
        }
        else {
            if (registers[i]->contain(s)) {
                ret = registers[i];
                ret->lastAccess = ++timeStamp;
                /* value in register should be consistent
                    with value in stack or memory.
                    However, no effective assetion could be easily
                    written here. In the future, it is recommended
                    to add some assertion here. */
                return ret->reg_name;
            }
            if (ret == NULL)
                ret = registers[i];
            if (ret->allocated && ret->lastAccess < registers[i]->lastAccess)
                ret = registers[i];
        }
        if (ret->allocated) release(ret);
        assign(ret, s, true);
        if (reload) load(ret, s);
        return ret->reg_name;
    }
    void passParam(string s, string param) {
        /* s can be number, variable, address */
        Register *cur = NULL, *reg_param = reg_ptr[param];
        for (int i = 0; i < Reg_N; i++)
            if (registers[i]->contain(s)) cur = registers[i];
        if (reg_param->contain(s)) return ;

        release(reg_param);
        /* it's safe to directly release, because no
            more register is needed in param pass */

        if (cur != NULL) 
            /* already in register */
            tiggerStmt(new _tDIRECT(reg_param->reg_name, cur->reg_name));
        else
            load(reg_param, s);
    }
    void caller_store() {
        for (int i = 0; i < )
    }
    void callee_store() {
        for (int i = 0; i < Reg_s; i++)
            reg_ptr["s" + to_string(i)]->store();
    }
};