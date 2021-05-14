#include <bits/stdc++.h>
#ifndef tiggerAST_hpp
#include "tiggerAST.hpp"
#define tiggerAST_hpp
#endif
using namespace std;

extern void print(string x) ;
extern void printTab(string x) ;

class eeyoreAST {
public:
    eeyoreAST() {}
    ~eeyoreAST() {}
    virtual void Dump() { assert(false); } // printCode
    virtual string getName() { assert(false); }
    virtual int getInt() { assert(false); }
    virtual void translate() { assert(false); } // translate AST of eeyoreAST into AST of tigger
    virtual bool isnum() { assert(false); } // for _ATOM, return true if it is an integer number
    virtual bool isdef() { return false; } // for convenience, "return false" ..
    virtual void try_allocate() { assert(false); }
};

/*
    Definition of nodes in Abstraction Syntax Tree of eeyore
        all specific nodes in eeyore AST has prefix _e,
        so as to avoid multiple global classes with same names.
    
    The whole structure is interpreted as
        sequence of statements, sequence of functions.
        and each function is also composed of sequential statements
    
    statements have several forms.
*/
class _eATOM: public eeyoreAST {
public:
    _eATOM() {}
};
class _eNUM: public _eATOM {
public:
    int num;
    _eNUM(int _num): num(_num) {}
    virtual string getName() { return to_string(num); }
    virtual int getInt() { return num; }
    virtual bool isnum() { return true; }
};
class _eVAR: public _eATOM {
public:
    string var;
    _eVAR(string _var): var(_var) {}
    virtual string getName() { return var; }
    virtual bool isnum() { return false; }
};

class _eDEFVAR: public eeyoreAST {
public:
    string var;
    _eDEFVAR(string _var): var(_var) {}
    virtual void Dump() { printTab("var " + var); }
    virtual bool isdef() { return true; }
    virtual void translate() ;
    virtual void try_allocate() ;
};
class _eDEFARR: public eeyoreAST {
public:
    string var; int size; // without * 4
    _eDEFARR(string _var, int _size): var(_var), size(_size) {}
    virtual void Dump() { printTab("var " + to_string(size * 4) + " " + var); }
    virtual bool isdef() { return true; }
    virtual void translate() ;
    virtual void try_allocate() ;
};
class _eDIRECT: public eeyoreAST {
public:
    eeyoreAST *a, *t;
    _eDIRECT(eeyoreAST *_a, eeyoreAST *_t): a(_a), t(_t) {}
    virtual void Dump() { printTab(a->getName() + " = " + t->getName()); }
    virtual void translate() ;
};
class _eUNARY: public eeyoreAST {
public:
    eeyoreAST *a, *t; string op;
    _eUNARY(eeyoreAST *_a, string _op, eeyoreAST *_t): a(_a), op(_op), t(_t) {}
    virtual void Dump() { printTab(a->getName() + " = " + op + t->getName()); }
    virtual void translate() ;
};
class _eBINARY: public eeyoreAST {
public:
    eeyoreAST *a, *t1, *t2; string op;
    _eBINARY(eeyoreAST *_a, eeyoreAST *_t1, string _op, eeyoreAST *_t2): a(_a), t1(_t1), op(_op), t2(_t2) {}
    virtual void Dump() { printTab(a->getName() + " = " + t1->getName() + " " + op + " " + t2->getName()); }
    virtual void translate() ;
};
class _eSEEK: public eeyoreAST {
public:
    eeyoreAST *a, *x; string t;
    _eSEEK(eeyoreAST *_a, string _t, eeyoreAST *_x): a(_a), t(_t), x(_x) {}
    virtual void Dump() { printTab(a->getName() + " = " + t + "[" + x->getName() + "]"); }
    virtual void translate() ;
};
class _eSAVE: public eeyoreAST {
public:
    eeyoreAST *t, *x; string a;
    _eSAVE(string _a, eeyoreAST *_x, eeyoreAST *_t): a(_a), x(_x), t(_t) {}
    virtual void Dump() { printTab(a + "[" + x->getName() + "] = " + t->getName()); }
    virtual void translate() ;
};
class _eFUNCRET: public eeyoreAST {
public:
    eeyoreAST *a; string func;
    _eFUNCRET(eeyoreAST *_a, string _func): a(_a), func(_func) {}
    virtual void Dump() { printTab(a->getName() + " = call " + func); }
    virtual void translate() ;
};
class _eIFGOTO: public eeyoreAST {
public:
    eeyoreAST *t1, *t2; string l; string op;
    _eIFGOTO(eeyoreAST *_t1, string _op, eeyoreAST *_t2, string _l): t1(_t1), op(_op), t2(_t2), l(_l) {}
    virtual void Dump() { printTab("if " + t1->getName() + " " + op + " " + t2->getName() + " goto " + l); }
    virtual void translate() ;
};
class _eGOTO: public eeyoreAST {
public:
    string l;
    _eGOTO(string _l): l(_l) {}
    virtual void Dump() { printTab("goto " + l); }
    virtual void translate() ;
};
class _ePARAM: public eeyoreAST {
public:
    eeyoreAST *t;
    _ePARAM(eeyoreAST *_t): t(_t) {}
    virtual void Dump() { printTab("param " + t->getName()); }
    virtual void translate() ;
};
class _eLABEL: public eeyoreAST {
public:
    string l;
    _eLABEL(string _l): l(_l) {}
    virtual void Dump() { print(l + ":"); }
    virtual void translate() ;
};
class _eRETVOID: public eeyoreAST {
public:
    _eRETVOID() {}
    virtual void Dump() { printTab("return"); }
    virtual void translate() ;
};
class _eRET: public eeyoreAST {
public:
    eeyoreAST *t;
    _eRET(eeyoreAST *_t): t(_t) {}
    virtual void Dump() { printTab("return " + t->getName()); }
    virtual void translate() ;
};
class _eFUNC: public eeyoreAST {
public:
    eeyoreAST *body; string func; int arity;
    _eFUNC(string _func, int _arity, eeyoreAST *_body): func(_func), arity(_arity), body(_body) {}
    virtual void Dump() { print(func + " [" + to_string(arity) + "]"); body->Dump(); print("end " + func); }
    virtual void translate() ;
};
class _eCALL: public eeyoreAST {
public:
    string func;
    _eCALL(string _func): func(_func) {}
    virtual void Dump() { printTab("call " + func); }
    virtual void translate() ;
};
class _eSEQ: public eeyoreAST {
public:
    vector<eeyoreAST *> seq;
    _eSEQ(vector<eeyoreAST *> _seq): seq(_seq) {}
    virtual void Dump() { for (auto stmt: seq) stmt->Dump(); }
    virtual void translate() ;
};