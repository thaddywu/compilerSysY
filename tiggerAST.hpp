#include <bits/stdc++.h>
using namespace std;

extern void print(string x) ;
extern void printTab(string x) ;
extern bool isreg(string x) ;
extern bool islogicop(string x) ;

class tiggerAST {
public:
    tiggerAST() {}
    ~tiggerAST() {}
    virtual void Dump() { assert(false); } // printCode
};

class _tGLBVAR: public tiggerAST {
public:
    string var;
    _tGLBVAR(string _var): var(_var) {}
    virtual void Dump() { print(var + " = 0"); }
};
class _tGLBARR: public tiggerAST {
public:
    string var; int size; // without * 4
    _tGLBARR(string _var, int _size): var(_var), size(_size) {}
    virtual void Dump() { print(var + " = malloc " + to_string(size * 4)); }
};
class _tFUNC: public tiggerAST {
public:
    tiggerAST *body; string func; int arity, mem; // mem without * 4
    _tFUNC(string _func, int _arity): func(_func), arity(_arity) { mem = 0; }
    _tFUNC(string _func, int _arity, int _mem, tiggerAST *_body): func(_func), arity(_arity), mem(_mem), body(_body) {}
    virtual void Dump() { print("f_" + func + " [" + to_string(arity) + "] [" + to_string(mem) + "]"); body->Dump(); print("end f_" + func); }
};
class _tSEQ: public tiggerAST {
public:
    vector<tiggerAST *> seq;
    _tSEQ(vector<tiggerAST *> _seq): seq(_seq) {}
    virtual void Dump() { for (auto stmt: seq) stmt->Dump(); }
};
class _tDIRECT: public tiggerAST {
public:
    string d, s;
    _tDIRECT(string _d, int _s): d(_d), s(to_string(_s)) { assert(isreg(d)); }
    _tDIRECT(string _d, string _s): d(_d), s(_s) { assert(isreg(d)); }
    virtual void Dump() { printTab(d + " = " + s); }
};
class _tUNARY: public tiggerAST {
public:
    string d, op, s;
    _tUNARY(string _d, string _op, string _s): d(_d), op(_op), s(_s) { assert(isreg(d) && isreg(s)); }
    virtual void Dump() { printTab(d + " = " + op + s); }
};
class _tBINARY: public tiggerAST {
public:
    string d, s, op, t;
    _tBINARY(string _d, string _s, string _op, string _t): d(_d), op(_op), s(_s), t(_t) { assert(isreg(d) && isreg(s)); }
    _tBINARY(string _d, string _s, string _op, int _t): d(_d), op(_op), s(_s), t(to_string(_t)) { assert(isreg(d) && isreg(s)); }
    virtual void Dump() { printTab(d + " = " + s + " " + op + " " + t); }
};
class _tSAVE: public tiggerAST {
public:
    string d, x, s;
    _tSAVE(string _d, int _x, string _s): d(_d), x(to_string(_x)), s(_s) { assert(isreg(d) && isreg(s)); }
    virtual void Dump() { printTab(d + "[" + x + "] = " + s); }
};
class _tSEEK: public tiggerAST {
public:
    string d, s, x;
    _tSEEK(string _d, string _s, int _x): d(_d), s(_s), x(to_string(_x)) { assert(isreg(d) && isreg(s)); }
    virtual void Dump() { printTab(d + " = " + s + "[" + x + "]"); }
};
class _tIFGOTO: public tiggerAST {
public:
    string s, op, t, l;
    _tIFGOTO(string _s, string _op, string _t, string _l): s(_s), op(_op), t(_t), l(_l) { assert(isreg(s) && isreg(t) && islogicop(op)); }
    virtual void Dump() { printTab("if " + s + " " + op + " " + t + " goto " + l); }
};
class _tGOTO: public tiggerAST {
public:
    string l;
    _tGOTO(string _l): l(_l) {}
    virtual void Dump() { printTab("goto " + l); }
};
class _tLABEL: public tiggerAST {
public:
    string l;
    _tLABEL(string _l): l(_l) {}
    virtual void Dump() { print(l + ":"); }
};
class _tCALL: public tiggerAST {
public:
    string func;
    _tCALL(string _func): func(_func) {}
    virtual void Dump() { printTab("call f_" + func); }
};
class _tRETURN: public tiggerAST {
public:
    _tRETURN() {}
    virtual void Dump() { printTab("return"); }
};
class _tLOAD: public tiggerAST {
public:
    string s, reg;
    _tLOAD(string _s, string _reg): s(_s), reg(_reg) { assert(!isreg(s) && isreg(reg)); }
    _tLOAD(int _s, string _reg): s(to_string(_s)), reg(_reg) { assert(!isreg(s) && isreg(reg)); }
    virtual void Dump() { printTab("load " + s + " " + reg); }
};
class _tLOADADDR: public tiggerAST {
public:
    string s, reg;
    _tLOADADDR(string _s, string _reg): s(_s), reg(_reg) { assert(!isreg(s) && isreg(reg)); }
    _tLOADADDR(int _s, string _reg): s(to_string(_s)), reg(_reg) { assert(!isreg(s) && isreg(reg)); }
    virtual void Dump() { printTab("loadaddr " + s + " " + reg); }
};
class _tSTORE: public tiggerAST {
public:
    string reg, s;
    _tSTORE(string _reg, int _s): reg(_reg), s(to_string(_s)) { assert(isreg(reg)); }
    virtual void Dump() { printTab("store " + reg + " " + s); }
};